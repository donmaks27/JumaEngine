// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/assets/AssetsEngineSubsystem.h"

#include "JumaEngine/engine/Engine.h"

namespace JumaEngine
{
	bool AssetsEngineSubsystem::initSubsystem()
	{
		if (!Super::initSubsystem())
        {
            return false;
        }

        JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
        renderEngine->registerVertexComponent(m_VertexComponentIDs.add(VertexComponent::Position2D, JSTR("position2D")), { 
            JumaRE::VertexComponentType::Vec2, 0
        });
        renderEngine->registerVertexComponent(m_VertexComponentIDs.add(VertexComponent::Position3D, JSTR("position3D")), { 
            JumaRE::VertexComponentType::Vec3, 0
        });
        renderEngine->registerVertexComponent(m_VertexComponentIDs.add(VertexComponent::TextureCoords, JSTR("textureCoords")), { 
            JumaRE::VertexComponentType::Vec2, 1
        });

        renderEngine->onDestroying.bind(this, &AssetsEngineSubsystem::onRenderEngineDestroying);
        return true;
	}
	void AssetsEngineSubsystem::clearSubsystem()
	{
		clear();
		Super::clearSubsystem();
	}
	void AssetsEngineSubsystem::clear()
	{
		JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
        if (renderEngine != nullptr)
        {
            renderEngine->onDestroying.unbind(this, &AssetsEngineSubsystem::onRenderEngineDestroying);
        }

        for (auto& mesh : m_Meshes)
        {
            mesh.clearMesh();
        }
        m_Meshes.clear();

        renderEngine->destroyVertexBuffer(m_VertexBuffer_Cube);
        renderEngine->destroyVertexBuffer(m_VertexBuffer_Plane2D);
        m_VertexBuffer_Cube = nullptr;
        m_VertexBuffer_Plane2D = nullptr;

        for (auto& material : m_Materials)
        {
            if (material.updatePtr() != nullptr)
            {
	            material->clearMaterial();
            }
        }
        m_Materials.clear();
        m_GlobalMaterialParams.clear();

        for (auto& shader : m_Shaders)
        {
            if (shader.value != nullptr)
            {
	            shader.value->clearShader();
            }
        }
        for (auto& shader : m_EngineShaders)
        {
            if (shader.value != nullptr)
            {
	            shader.value->clearShader();
            }
        }
        m_Shaders.clear();
        m_EngineShaders.clear();
        
        for (auto& texture : m_Textures)
        {
            if (texture.value != nullptr)
            {
	            texture.value->clearTexture();
            }
        }
        for (auto& texture : m_EngineTextures)
        {
            if (texture.value != nullptr)
            {
	            texture.value->clearTexture();
            }
        }
        m_Textures.clear();
        m_EngineTextures.clear();
	}

	const EngineObjectPtr<Texture>& AssetsEngineSubsystem::getEngineTexture(const jstringID& textureName)
	{
        return getTexture(m_EngineTextures, textureName, getEngine()->getEngineContentDirectory());
	}
	const EngineObjectPtr<Texture>& AssetsEngineSubsystem::getTexture(const jstringID& textureName)
	{
        return getTexture(m_Textures, textureName, getEngine()->getGameContentDirectory());
	}
	const EngineObjectPtr<Texture>& AssetsEngineSubsystem::getTexture(jmap<jstringID, EngineObjectPtr<Texture>>& texturesList, const jstringID& textureName, const jstring& contentFolder) const
	{
        const EngineObjectPtr<Texture>* texturePtr = texturesList.find(textureName);
        if (texturePtr != nullptr)
        {
	        return *texturePtr;
        }

        EngineObjectPtr<Texture>& texture = texturesList.add(textureName, getEngine()->createObject<Texture>());
        if (!texture->loadTexture(textureName, contentFolder))
        {
            JUTILS_LOG(error, JSTR("Failed to load texture {} from {}"), textureName.toString(), contentFolder);
            texture = nullptr;
            return texture;
        }

        JUTILS_LOG(correct, JSTR("Loaded texture {} from {}"), textureName.toString(), contentFolder);
        return texture;
	}

    const EngineObjectPtr<Shader>& AssetsEngineSubsystem::getEngineShader(const jstringID& shaderName)
	{
        return getShader(m_EngineShaders, shaderName, getEngine()->getEngineContentDirectory());
	}
    const EngineObjectPtr<Shader>& AssetsEngineSubsystem::getShader(const jstringID& shaderName)
    {
        return getShader(m_Shaders, shaderName, getEngine()->getGameContentDirectory());
    }
    const EngineObjectPtr<Shader>& AssetsEngineSubsystem::getShader(jmap<jstringID, EngineObjectPtr<Shader>>& shadersList, const jstringID& shaderName, const jstring& contentFolder) const
	{
        EngineObjectPtr<Shader>* shaderPtr = shadersList.find(shaderName);
        if (shaderPtr != nullptr)
        {
            return *shaderPtr;
        }

        EngineObjectPtr<Shader>& shader = shadersList.add(shaderName, getEngine()->createObject<Shader>());
        if (!shader->loadShader(shaderName, contentFolder))
        {
            JUTILS_LOG(error, JSTR("Failed to create shader {} from {}"), shaderName.toString(), contentFolder);
            shader = nullptr;
            return shader;
        }
        JUTILS_LOG(correct, JSTR("Created shader {} from {}"), shaderName.toString(), contentFolder);
        return shader;
	}

    EngineObjectPtr<Material> AssetsEngineSubsystem::createMaterial(const EngineObjectPtr<Shader>& shader)
    {
        EngineObjectPtr<Material> material = getEngine()->createObject<Material>();
        if (!material->createMaterial(shader))
        {
            JUTILS_LOG(error, JSTR("Failed to create material from shader {}"), shader != nullptr ? shader->getName().toString() : JSTR("NULL"));
            return nullptr;
        }
        m_Materials.add(material);
        return material;
    }
    EngineObjectPtr<Material> AssetsEngineSubsystem::createMaterial(const EngineObjectPtr<Material>& baseMaterial)
    {
        EngineObjectPtr<Material> material = getEngine()->createObject<Material>();
        if (!material->createMaterial(baseMaterial))
        {
            JUTILS_LOG(error, JSTR("Failed to create material instance"));
            return nullptr;
        }
        m_Materials.add(material);
        return material;
    }

    jstringID AssetsEngineSubsystem::getVertexComponentID(const VertexComponent component) const
	{
        const jstringID* componentID = m_VertexComponentIDs.find(component);
        return componentID != nullptr ? *componentID : jstringID_NONE;
	}
    JumaRE::VertexBuffer* AssetsEngineSubsystem::getVertexBuffer_Cube()
    {
        if (m_VertexBuffer_Cube != nullptr)
        {
	        return m_VertexBuffer_Cube;
        }

        JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
        if (renderEngine == nullptr)
        {
            JUTILS_LOG(error, JSTR("Can't get render engine"));
            return nullptr;
        }

        struct vertex_data
        {
            math::vector3 position;
            math::vector2 textureCoords;
        };
        const jarray<vertex_data> data = {
            { { -1.0f,  1.0f,  1.0f }, { 0.0f, 0.0f } },
            { { -1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f } },
            { {  1.0f,  1.0f, -1.0f }, { 1.0f, 1.0f } },
            { {  1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f } },
            
            { {  1.0f,  1.0f,  1.0f }, { 0.0f, 0.0f } },
            { {  1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f } },
            { {  1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f } },
            { {  1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f } },
            
            { {  1.0f, -1.0f,  1.0f }, { 0.0f, 0.0f } },
            { {  1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f } },
            { { -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f } },
            { { -1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f } },
            
            { { -1.0f, -1.0f,  1.0f }, { 0.0f, 0.0f } },
            { { -1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f } },
            { { -1.0f,  1.0f, -1.0f }, { 1.0f, 1.0f } },
            { { -1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f } },
            
            { { -1.0f, -1.0f,  1.0f }, { 0.0f, 0.0f } },
            { { -1.0f,  1.0f,  1.0f }, { 0.0f, 1.0f } },
            { {  1.0f,  1.0f,  1.0f }, { 1.0f, 1.0f } },
            { {  1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f } },
            
            { { -1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f } },
            { { -1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f } },
            { {  1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f } },
            { {  1.0f,  1.0f, -1.0f }, { 1.0f, 0.0f } }
        };
        const jarray<uint32> indices = {
             0,  1,  2,  0,  2,  3,
             4,  5,  6,  4,  6,  7,
             8,  9, 10,  8, 10, 11,
            12, 13, 14, 12, 14, 15,
            16, 17, 18, 16, 18, 19,
            20, 21, 22, 20, 22, 23
        };

        m_VertexBuffer_Cube = renderEngine->createVertexBuffer(JumaRE::MakeVertexBufferData({ {
            getVertexComponentID(VertexComponent::Position3D), getVertexComponentID(VertexComponent::TextureCoords)
        } }, data, indices ));
        return m_VertexBuffer_Cube;
    }
    JumaRE::VertexBuffer* AssetsEngineSubsystem::getVertexBuffer_Plane2D()
    {
        if (m_VertexBuffer_Plane2D != nullptr)
        {
	        return m_VertexBuffer_Plane2D;
        }

        JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
        if (renderEngine == nullptr)
        {
            JUTILS_LOG(error, JSTR("Can't get render engine"));
            return nullptr;
        }

        struct vertex_data
        {
            math::vector2 position;
            math::vector2 textureCoords;
        };
        const jarray<vertex_data> data = {
            { jutils::math::vector2{ 0.0f, 0.0f }, { 0.0f, 0.0f } },
            { jutils::math::vector2{ 0.0f, 1.0f }, { 0.0f, 1.0f } },
            { jutils::math::vector2{ 1.0f, 0.0f }, { 1.0f, 0.0f } },
            { jutils::math::vector2{ 1.0f, 0.0f }, { 1.0f, 0.0f } },
            { jutils::math::vector2{ 0.0f, 1.0f }, { 0.0f, 1.0f } },
            { jutils::math::vector2{ 1.0f, 1.0f }, { 1.0f, 1.0f } }
        };

        m_VertexBuffer_Plane2D = renderEngine->createVertexBuffer(JumaRE::MakeVertexBufferData({ {
            getVertexComponentID(VertexComponent::Position2D), getVertexComponentID(VertexComponent::TextureCoords)
        } }, data));
        return m_VertexBuffer_Plane2D;
    }
}
