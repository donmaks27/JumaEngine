// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "../../../include/JumaEngine/subsystems/meshes/MeshesSubsystem.h"

#include <jutils/math/vector3.h>

#include "../../../include/JumaEngine/Engine.h"

namespace JumaEngine
{
    bool MeshesSubsystem::initSubsystem()
    {
        if (!Super::initSubsystem())
        {
            return false;
        }

        JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
        renderEngine->registerVertexComponent(JSTR("position2D"), { JumaRE::VertexComponentType::Vec2, 0 });
        renderEngine->registerVertexComponent(JSTR("position3D"), { JumaRE::VertexComponentType::Vec3, 0 });
        renderEngine->registerVertexComponent(JSTR("textureCoords"), { JumaRE::VertexComponentType::Vec2, 1 });
        return true;
    }
    void MeshesSubsystem::clearSubsystem()
    {
        clear();

        Super::clearSubsystem();
    }

    void MeshesSubsystem::clear()
    {
        for (auto& mesh : m_Meshes)
        {
            mesh.clearMesh();
        }
        m_Meshes.clear();
    }

    Mesh* MeshesSubsystem::generateCudeMesh(Material* material)
    {
        struct vertex_data
        {
            math::vector3 position;
            math::vector2 textureCoords;
        };
        jarray<vertex_data> data = {
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
        switch (getEngine()->getRenderEngine()->getRenderAPI())
        {
        case JumaRE::RenderAPI::OpenGL:
            {
                for (auto& vertex : data)
                {
                    vertex.textureCoords.y = 1.0f - vertex.textureCoords.y;
                }
            }
            break;
        default: ;
        }

        Mesh* mesh = getEngine()->registerObject(&m_Meshes.addDefault());
        const bool meshValid = mesh->init({ 
            JumaRE::MakeVertexBufferData({ { JSTR("position3D"), JSTR("textureCoords") } }, data, indices )
        }, { material });
        if (!meshValid)
        {
            m_Meshes.removeLast();
            return nullptr;
        }
        return mesh;
    }
    Mesh* MeshesSubsystem::generatePlane2DMesh(Material* material)
    {
        struct vertex_data
        {
            math::vector2 position;
            math::vector2 textureCoords;
        };

        const JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
        if (renderEngine == nullptr)
        {
            return nullptr;
        }

        jarray<vertex_data> data = {
            { jutils::math::vector2{ -1.0f, -1.0f }, { 0.0f, 0.0f } },
            { jutils::math::vector2{ -1.0f,  1.0f }, { 0.0f, 1.0f } },
            { jutils::math::vector2{  1.0f, -1.0f }, { 1.0f, 0.0f } },
            { jutils::math::vector2{  1.0f, -1.0f }, { 1.0f, 0.0f } },
            { jutils::math::vector2{ -1.0f,  1.0f }, { 0.0f, 1.0f } },
            { jutils::math::vector2{  1.0f,  1.0f }, { 1.0f, 1.0f } }
        };
        switch (renderEngine->getRenderAPI())
        {
        case JumaRE::RenderAPI::DirectX11:
        case JumaRE::RenderAPI::DirectX12:
            {
                for (auto& vertex : data)
                {
                    vertex.position.y = -vertex.position.y;
                }
            }
            break;
        case JumaRE::RenderAPI::OpenGL:
            {
                for (auto& vertex : data)
                {
                    vertex.textureCoords.y = 1.0f - vertex.textureCoords.y;
                }
            }
            break;
        default: ;
        }

        Mesh* mesh = getEngine()->registerObject(&m_Meshes.addDefault());
        const bool meshValid = mesh->init({ 
            JumaRE::MakeVertexBufferData({ { JSTR("position2D"), JSTR("textureCoords") } }, data )
        }, { material });
        if (!meshValid)
        {
            m_Meshes.removeLast();
            return nullptr;
        }
        return mesh;
    }
}
