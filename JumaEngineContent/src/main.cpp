// Copyright © 2023 Leonov Maksim. All rights reserved.

#include <filesystem>
#include <fstream>

#include <JumaShaderCompiler/Compiler.h>
#include <jutils/log.h>
#include <jutils/jdefer.h>
using namespace jutils;

bool getShaderType(const jstring& extension, JumaSC::GLSL::type& outType);
bool getShaderTypeHLSL(JumaSC::GLSL::type type, JumaSC::HLSL::type& outType);

int main(const int argCount, const char* args[])
{
    jarray<jstring> arguments(argCount);
    for (int32 index = 0; index < argCount; index++)
    {
        arguments[index] = args[index];
    }
    const int32 contentPathIndex = arguments.indexOf([](const jstring& argument){ return argument.startsWith("-C"); });
    const jstring contentPath = (contentPathIndex >= 0) && (arguments[contentPathIndex].getSize() > 2) ?
        arguments[contentPathIndex].substr(2).replaceAll('\\', '/') : "../../../JumaEngine/content_engine/";
    const jstring shadersPath = contentPath + (contentPath.endsWith('/') ? "shaders/" : "/shaders/");

    JumaSC::Compiler* compiler = JumaSC::Compiler::Create();
    if (compiler == nullptr)
    {
        JUTILS_LOG(error, "failed to create JumaSC instance");
        return -1;
    }
    const bool enabledGLSL = compiler->isGlslCompileEnabled();
    const bool enabledSpvToHLSL = compiler->isSpvToHlslEnabled();
    const bool enabledHLSL = compiler->isHlslCompileEnabled();
    for (const auto& entry : std::filesystem::directory_iterator(*shadersPath))
    {
        if (entry.is_directory())
        {
            continue;
        }
        if (!entry.path().has_extension())
        {
            continue;
        }
        JumaSC::GLSL::type shaderType;
        if (!getShaderType(entry.path().extension().string(), shaderType))
        {
            continue;
        }

        JUTILS_LOG(info, "compiling shader {}", entry.path().string());

        if (!enabledGLSL)
        {
            continue;
        }
        std::ifstream shaderFileGLSL(entry.path());
        if (!shaderFileGLSL.is_open())
        {
            JUTILS_LOG(warning, "failed to open shader type (file {})", entry.path().string());
            continue;
        }
        jarray<jstring> shaderTextGLSL;
        std::string line;
        while (std::getline(shaderFileGLSL, line))
        {
            shaderTextGLSL += line + '\n';
        }
        shaderFileGLSL.close();

        const jarray<uint32> shaderSPV = compiler->glslToSPV(shaderTextGLSL, shaderType);
        if (shaderSPV.isEmpty())
        {
            JUTILS_LOG(warning, "failed to compile GLSL shader (file {})", entry.path().string());
            continue;
        }
        std::ofstream shaderFileSPV(entry.path().string() + ".spv");
        for (const auto& data : shaderSPV)
        {
            shaderFileSPV.write(reinterpret_cast<const char*>(&data), sizeof(uint32));
        }
        shaderFileSPV.close();

        if (!enabledSpvToHLSL)
        {
            continue;
        }
        const jstring shaderTextHLSL = compiler->hlslFromSPV(shaderSPV, JumaShaderCompiler::HLSL::model::_6_0);
        if (shaderTextHLSL.isEmpty())
        {
            JUTILS_LOG(warning, "failed to generate HLSL shader (file {})", entry.path().string());
            continue;
        }
        std::ofstream shaderFileHLSL(entry.path().string() + ".hlsl");
        shaderFileHLSL << *shaderTextHLSL;
        shaderFileHLSL.close();

        if (!enabledHLSL)
        {
            continue;
        }
        JumaSC::HLSL::type shaderTypeHLSL;
        if (!getShaderTypeHLSL(shaderType, shaderTypeHLSL))
        {
            continue;
        }
        const jarray<uint8> shaderHLSL = compiler->hlslCompile({ shaderTextHLSL }, shaderTypeHLSL, JumaShaderCompiler::HLSL::model::_6_0);
        if (shaderHLSL.isEmpty())
        {
            JUTILS_LOG(warning, "failed to compile HLSL shader (file {})", entry.path().string());
            continue;
        }
        std::ofstream shaderFileBinHLSL(entry.path().string() + ".hlsl.obj");
        for (const auto& data : shaderHLSL)
        {
            shaderFileBinHLSL.write(reinterpret_cast<const char*>(&data), sizeof(uint8));
        }
        shaderFileBinHLSL.close();
    }
    delete compiler;
    return 0;
}

bool getShaderType(const jstring& extension, JumaSC::GLSL::type& outType)
{
    if (extension == ".vert")
    {
        outType = JumaShaderCompiler::GLSL::type::vertex;
    }
    else if (extension == ".frag")
    {
        outType = JumaShaderCompiler::GLSL::type::fragment;
    }
    else if (extension == ".geom")
    {
        outType = JumaShaderCompiler::GLSL::type::geometry;
    }
    else if (extension == ".comp")
    {
        outType = JumaShaderCompiler::GLSL::type::compute;
    }
    else
    {
        return false;
    }
    return true;
}

bool getShaderTypeHLSL(const JumaSC::GLSL::type type, JumaSC::HLSL::type& outType)
{
    switch (type)
    {
    case JumaShaderCompiler::GLSL::type::vertex:   outType = JumaSC::HLSL::type::vertex; break;
    case JumaShaderCompiler::GLSL::type::fragment: outType = JumaSC::HLSL::type::fragment; break;
    case JumaShaderCompiler::GLSL::type::geometry: outType = JumaSC::HLSL::type::geometry; break;
    case JumaShaderCompiler::GLSL::type::compute:  outType = JumaSC::HLSL::type::compute; break;
    default:
        return false;
    }
    return true;
}