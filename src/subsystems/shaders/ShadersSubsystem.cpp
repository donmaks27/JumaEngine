// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "../../../include/JumaEngine/subsystems/shaders/ShadersSubsystem.h"

#include "../../../include/JumaEngine/Engine.h"

namespace JumaEngine
{
    bool ShadersSubsystem::initSubsystem()
    {
        if (!Super::initSubsystem())
        {
            return false;
        }

        getEngine()->getRenderEngine()->onDestroying.bind(this, &ShadersSubsystem::onRenderEngineDestroying);

        m_EngineInternalParams.setValue<JumaRE::ShaderUniformType::Vec2>(JSTR("ScreenCoordsModifier"), getEngine()->getRenderEngine()->getScreenCoordinateModifier());
        return true;
    }
    void ShadersSubsystem::clearSubsystem()
    {
        clear();

        Super::clearSubsystem();
    }

    Shader* ShadersSubsystem::getShader(const jstringID& shaderName)
    {
        Shader* shaderPtr = m_Shaders.find(shaderName);
        if (shaderPtr != nullptr)
        {
            return shaderPtr;
        }

        Shader* shader = getEngine()->registerObject(&m_Shaders.add(shaderName));
        if (!shader->loadShader(shaderName))
        {
            JUTILS_LOG(error, JSTR("Failed to create shader {}"), shaderName.toString());
            m_Shaders.remove(shaderName);
            return nullptr;
        }

        JUTILS_LOG(correct, JSTR("Created shader {}"), shaderName.toString());
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
