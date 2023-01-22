// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/subsystems/shaders/ShadersSubsystem.h"

#include "JumaEngine/Engine.h"

namespace JumaEngine
{
    bool ShadersSubsystem::initSubsystem()
    {
        if (!Super::initSubsystem())
        {
            return false;
        }

        getEngine()->getRenderEngine()->onDestroying.bind(this, &ShadersSubsystem::onRenderEngineDestroying);
        return true;
    }
    void ShadersSubsystem::clearSubsystem()
    {
        clear();

        Super::clearSubsystem();
    }

    Shader* ShadersSubsystem::getEngineShader(const jstringID& shaderName)
    {
        static jstringID contentFolder = JSTR("content_engine");
        return getShader(m_EngineShaders, shaderName, contentFolder);
    }
    Shader* ShadersSubsystem::getShader(const jstringID& shaderName)
    {
        static jstringID contentFolder = JSTR("content");
        return getShader(m_Shaders, shaderName, contentFolder);
    }
    Shader* ShadersSubsystem::getShader(jmap<jstringID, Shader>& shadersList, const jstringID& shaderName, const jstringID& contentFolder) const
    {
        Shader* shaderPtr = shadersList.find(shaderName);
        if (shaderPtr != nullptr)
        {
            return shaderPtr;
        }

        Shader* shader = getEngine()->registerObject(&shadersList.add(shaderName));
        if (!shader->loadShader(shaderName, contentFolder))
        {
            JUTILS_LOG(error, JSTR("Failed to create shader {} from {}"), shaderName.toString(), contentFolder.toString());
            shadersList.remove(shaderName);
            return nullptr;
        }

        JUTILS_LOG(correct, JSTR("Created shader {} from {}"), shaderName.toString(), contentFolder.toString());
        return shader;
    }

    Material* ShadersSubsystem::createMaterial(Shader* shader)
    {
        Material* material = getEngine()->registerObject(&m_Materials.addDefault());
        if (!material->createMaterial(shader))
        {
            JUTILS_LOG(error, JSTR("Failed to create material from shader "), shader != nullptr ? shader->getName().toString() : JSTR("NULL"));
            m_Materials.removeLast();
            return nullptr;
        }
        material->onClear.bind(this, &ShadersSubsystem::onMaterialClear);
        return material;
    }
    Material* ShadersSubsystem::createMaterial(Material* baseMaterial)
    {
        Material* material = getEngine()->registerObject(&m_Materials.addDefault());
        if (!material->createMaterial(baseMaterial))
        {
            JUTILS_LOG(error, JSTR("Failed to create material instance"));
            m_Materials.removeLast();
            return nullptr;
        }
        material->onClear.bind(this, &ShadersSubsystem::onMaterialClear);
        return material;
    }
    void ShadersSubsystem::destroyMaterial(Material* material)
    {
        if (material != nullptr)
        {
            material->clearMaterial();

            m_Materials.removeByPredicate([this](const Material& materialObject)
            {
                const int32 index = m_DestroyingMaterials.indexOf(&materialObject);
                if (index == -1)
                {
                    return false;
                }

                m_DestroyingMaterials.removeAt(index);
                return true;
            });
            m_DestroyingMaterials.clear();
        }
    }
    void ShadersSubsystem::onMaterialClear(Material* material)
    {
        material->onClear.unbind(this, &ShadersSubsystem::onMaterialClear);
        m_DestroyingMaterials.add(material);
    }

    void ShadersSubsystem::clear()
    {
        JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
        if (renderEngine != nullptr)
        {
            renderEngine->onDestroying.unbind(this, &ShadersSubsystem::onRenderEngineDestroying);
        }

        for (auto& material : m_Materials)
        {
            material.clearMaterial();
        }
        m_DestroyingMaterials.clear();
        m_Materials.clear();

        for (auto& shader : m_Shaders)
        {
            shader.value.clearShader();
        }
        m_Shaders.clear();

        m_EngineInternalParams.clear();
    }
}
