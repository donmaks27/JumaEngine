// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/assets/AssetsEngineSubsystem.h"

#include "JumaEngine/Engine.h"

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
            material.clearMaterial();
        }
        m_DestroyingMaterials.clear();
        m_Materials.clear();
        m_GlobalMaterialParams.clear();

        for (auto& shader : m_Shaders)
        {
            shader.value.clearShader();
        }
        m_Shaders.clear();

        for (auto& texture : m_Textures)
        {
            texture.value.clearTexture();
        }
        m_Textures.clear();
	}

	Texture* AssetsEngineSubsystem::getEngineTexture(const jstringID& textureName)
	{
		static jstringID contentFolder = JSTR("content_engine");
        return getTexture(m_EngineTextures, textureName, contentFolder);
	}
	Texture* AssetsEngineSubsystem::getTexture(const jstringID& textureName)
	{
		static jstringID contentFolder = JSTR("content");
        return getTexture(m_Textures, textureName, contentFolder);
	}
	Texture* AssetsEngineSubsystem::getTexture(jmap<jstringID, Texture>& texturesList, const jstringID& textureName, const jstringID& contentFolder) const
	{
		Texture* texturePtr = texturesList.find(textureName);
        if (texturePtr != nullptr)
        {
            return texturePtr;
        }

        Texture* texture = getEngine()->registerObject(&texturesList.add(textureName));
        if (!texture->loadTexture(textureName, contentFolder))
        {
            JUTILS_LOG(error, JSTR("Failed to load texture {} from {}"), textureName.toString(), contentFolder.toString());
            return nullptr;
        }

        JUTILS_LOG(correct, JSTR("Loaded texture {} from {}"), textureName.toString(), contentFolder.toString());
        return texture;
	}

    Shader* AssetsEngineSubsystem::getEngineShader(const jstringID& shaderName)
	{
        static jstringID contentFolder = JSTR("content_engine");
        return getShader(m_EngineShaders, shaderName, contentFolder);
	}
    Shader* AssetsEngineSubsystem::getShader(const jstringID& shaderName)
    {
        static jstringID contentFolder = JSTR("content");
        return getShader(m_Shaders, shaderName, contentFolder);
    }
    Shader* AssetsEngineSubsystem::getShader(jmap<jstringID, Shader>& shadersList, const jstringID& shaderName, const jstringID& contentFolder) const
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

    Material* AssetsEngineSubsystem::createMaterial(Shader* shader)
    {
        Material* material = getEngine()->registerObject(&m_Materials.addDefault());
        if (!material->createMaterial(shader))
        {
            JUTILS_LOG(error, JSTR("Failed to create material from shader "), shader != nullptr ? shader->getName().toString() : JSTR("NULL"));
            m_Materials.removeLast();
            return nullptr;
        }
        material->onClear.bind(this, &AssetsEngineSubsystem::onMaterialClear);
        return material;
    }
    Material* AssetsEngineSubsystem::createMaterial(Material* baseMaterial)
    {
        Material* material = getEngine()->registerObject(&m_Materials.addDefault());
        if (!material->createMaterial(baseMaterial))
        {
            JUTILS_LOG(error, JSTR("Failed to create material instance"));
            m_Materials.removeLast();
            return nullptr;
        }
        material->onClear.bind(this, &AssetsEngineSubsystem::onMaterialClear);
        return material;
    }
    void AssetsEngineSubsystem::destroyMaterial(Material* material)
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
    void AssetsEngineSubsystem::onMaterialClear(Material* material)
    {
        material->onClear.unbind(this, &AssetsEngineSubsystem::onMaterialClear);
        m_DestroyingMaterials.add(material);
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
