// Copyright © 2023 Leonov Maksim. All rights reserved.

#include "JumaEngineContent/shaders.h"

#include <filesystem>
#include <fstream>

#include <jutils/log.h>
#include <jutils/jdefer.h>
#include <jutils/jlist.h>
#include <jutils/configs/ini_parser.h>
#include <jutils/configs/json_parser.h>
#include <JumaShaderCompiler/Compiler.h>
using namespace jutils;

namespace JumaEngineContent
{
    bool getGLSLShaderTypeByFilename(const std::filesystem::path& filepath, JumaSC::GLSL::type& outType);
    jarray<uint32> compileGLSL(JumaSC::Compiler* compiler, const std::filesystem::path& filepath, JumaSC::GLSL::type type);
    jstring compileSpvToHLSL(JumaSC::Compiler* compiler, const std::filesystem::path& filepath, const jarray<uint32>& spv);

    void compileShaders(const jstring& type, const bool forceRecompile)
    {
        static const jstringID cacheFieldName = "cache";
        static const jstringID compileTimeFieldName = "compileTime";

        const jstring configPath = "./config/" + type + ".ini";
        const auto configData = ini::parseFile(configPath);
        if (configData.isEmpty())
        {
            JUTILS_LOG(warning, "can't find file or empty config file \"{}\"", configPath);
            return;
        }

        const auto section = configData.find("General");
        const auto value = section != nullptr ? section->find("contentFolder") : nullptr;
        if (value == nullptr)
        {
            JUTILS_LOG(warning, "can't find \"contentFolder\" key in config file {}", configPath);
            return;
        }

        const jstring contentFolderPath = value->endsWith('/') || value->endsWith('\\') ? *value : (*value + '/');
        if (!std::filesystem::is_directory(contentFolderPath.toBase()))
        {
            JUTILS_LOG(warning, "failed to find content folder \"{}\"", contentFolderPath);
            return;
        }
        const jstring shadersFolderPath = contentFolderPath + "shaders";
        if (!std::filesystem::is_directory(shadersFolderPath.toBase()))
        {
            JUTILS_LOG(warning, "can't find content shaders folder \"{}\"", shadersFolderPath);
            return;
        }

        JumaSC::Compiler* compiler = JumaSC::Compiler::Create();
        if (compiler == nullptr)
        {
            JUTILS_LOG(error, "failed to create JumaSC compiler");
            return;
        }
        jdefer deleteCompiler([compiler](){ delete compiler; });
        if (!compiler->isGlslCompileEnabled())
        {
            JUTILS_LOG(warning, "GLSL compiling in JumaSC compiler is disabled, can't do anything");
            return;
        }
        const bool spvToHlslEnabled = compiler->isSpvToHlslEnabled();
        //const bool hlslCompileEnabled = spvToHlslEnabled && compiler->isHlslCompileEnabled();

        jlist<std::filesystem::path> shaderFolders = { shadersFolderPath.toBase() };
        while (!shaderFolders.isEmpty())
        {
            const auto shaderFolder = shaderFolders.getFirst();
            shaderFolders.removeFirst();

            json::value_object shadersCacheConfigObject = json::castValue<json::type::object>(json::parseFile(shaderFolder.string() + "/shadersCacheValue.json"));
            if (shadersCacheConfigObject == nullptr)
            {
                shadersCacheConfigObject = json::createValue<json::type::object>();
            }
            json::value_object shadersCacheObject = nullptr;
            {
                json::value* shadersCacheValue = shadersCacheConfigObject->get().find(cacheFieldName);
                if (shadersCacheValue != nullptr)
                {
                    shadersCacheObject = json::castValue<json::type::object>(*shadersCacheValue);
                }
                if (shadersCacheObject == nullptr)
                {
                    shadersCacheObject = json::createValue<json::type::object>();
                    shadersCacheConfigObject->get().add(cacheFieldName, shadersCacheObject);
                }
            }

            uint32 updatedShaderCount = 0;
            for (const auto& entry : std::filesystem::directory_iterator(shaderFolder))
            {
                const std::filesystem::path& entryPath = entry.path();
                if (entry.is_directory())
                {
                    shaderFolders.add(entryPath);
                    continue;
                }
                if (!entry.is_regular_file())
                {
                    continue;
                }

                JumaSC::GLSL::type typeGLSL;
                if (!getGLSLShaderTypeByFilename(entryPath, typeGLSL))
                {
                    continue;
                }
                const jstringID shaderNameField = entryPath.filename().c_str();
                json::value_object shaderCacheObject = nullptr;
                {
                    json::value* shaderCacheValue = shadersCacheObject->get().find(shaderNameField);
                    if (shaderCacheValue != nullptr)
                    {
                        shaderCacheObject = json::castValue<json::type::object>(*shaderCacheValue);
                    }
                    if (shaderCacheObject == nullptr)
                    {
                        shaderCacheObject = json::createValue<json::type::object>();
                        shadersCacheObject->get().add(shaderNameField, shaderCacheObject);
                    }
                }

                json::value_number shaderLastWriteTime = nullptr;
                {
                    json::value* shaderCompileTimeValue = shaderCacheObject->get().find(compileTimeFieldName);
                    if (shaderCompileTimeValue != nullptr)
                    {
                        shaderLastWriteTime = json::castValue<json::type::number>(*shaderCompileTimeValue);
                    }
                }
                const auto lastWriteTime = std::chrono::file_clock::to_sys(entry.last_write_time());
                const uint32 lastWriteUtcTime = static_cast<uint32>(std::chrono::time_point_cast<std::chrono::seconds>(lastWriteTime).time_since_epoch().count());
                if (!forceRecompile && (shaderLastWriteTime != nullptr))
                {
                    if (math::round<uint32>(shaderLastWriteTime->get()) == lastWriteUtcTime)
                    {
                        continue;
                    }
                }

                JUTILS_LOG(info, "compiling file \"{}\"...", entryPath.c_str());
                const auto spvShader = compileGLSL(compiler, entryPath, typeGLSL);
                if (spvShader.isEmpty())
                {
                    JUTILS_LOG(error, "compiling GLSL failed");
                    continue;
                }
                if (spvToHlslEnabled)
                {
                    const auto hlslShader = compileSpvToHLSL(compiler, entryPath, spvShader);
                    if (hlslShader.isEmpty())
                    {
                        JUTILS_LOG(error, "compiling HLSL from SPV failed");
                        continue;
                    }
                }

                updatedShaderCount++;
                if (shaderLastWriteTime == nullptr)
                {
                    shaderCacheObject->get().add(compileTimeFieldName, json::createValue<json::type::number>(lastWriteUtcTime));
                }
                else
                {
                    shaderLastWriteTime->set(lastWriteUtcTime);
                }
            }

            if (updatedShaderCount > 0)
            {
                const jstring config = json::serialize(shadersCacheConfigObject, true);
                if (!config.isEmpty())
                {
                    std::ofstream file(shaderFolder.string() + "/shadersCacheValue.json");
                    file << *config;
                    file.close();
                }
            }
        }
    }

    bool getGLSLShaderTypeByFilename(const std::filesystem::path& filepath, JumaSC::GLSL::type& outType)
    {
        const std::filesystem::path extension = filepath.extension();
        if (extension == ".vert")
        {
            outType = JumaSC::GLSL::type::vertex;
            return true;
        }
        if (extension == ".frag")
        {
            outType = JumaShaderCompiler::GLSL::type::fragment;
            return true;
        }
        if (extension == ".geom")
        {
            outType = JumaShaderCompiler::GLSL::type::geometry;
            return true;
        }
        if (extension == ".comp")
        {
            outType = JumaShaderCompiler::GLSL::type::compute;
            return true;
        }
        return false;
    }

    jarray<uint32> compileGLSL(JumaSC::Compiler* compiler, const std::filesystem::path& filepath, const JumaSC::GLSL::type type)
    {
        std::ifstream file(filepath);
        if (!file.is_open())
        {
            JUTILS_LOG(error, "failed to open file \"{}\"", filepath.c_str());
            return {};
        }
        jarray<jstring> text;
        std::string line;
        while (std::getline(file, line))
        {
            text.add(line + '\n');
        }
        file.close();

        jarray<uint32> spv = compiler->glslToSPV(text, type, JumaSC::Vulkan::version::_1_3);
        if (!spv.isEmpty())
        {
            std::ofstream spvFile(filepath.string() + ".spv");
            for (const auto& data : spv)
            {
                spvFile.write(reinterpret_cast<const char*>(&data), sizeof(uint32));
            }
            spvFile.close();
        }
        return spv;
    }

    jstring compileSpvToHLSL(JumaSC::Compiler* compiler, const std::filesystem::path& filepath, const jarray<uint32>& spv)
    {
        const jstring hlsl = compiler->hlslFromSPV(spv, JumaShaderCompiler::HLSL::model::_6_0);
        if (!hlsl.isEmpty())
        {
            std::ofstream file(filepath.string() + ".hlsl");
            file << *hlsl;
            file.close();
        }
        return hlsl;
    }
}
