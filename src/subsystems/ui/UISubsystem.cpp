// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/subsystems/ui/UISubsystem.h"

#include "JumaEngine/Engine.h"
#include "JumaEngine/subsystems/meshes/MeshesSubsystem.h"

namespace JumaEngine
{
    bool UISubsystem::initSubsystem()
    {
        struct vertex_data
        {
            math::vector2 position;
            math::vector2 textureCoords;
        };

        if (!Super::initSubsystem())
        {
            return false;
        }

        const Engine* engine = getEngine();
        const MeshesSubsystem* meshesSubsystem = engine->getSubsystem<MeshesSubsystem>();
        if (meshesSubsystem == nullptr)
        {
            JUTILS_LOG(error, JSTR("Can't get MeshesSubsystem"));
            return false;
        }
        JumaRE::RenderEngine* renderEngine = engine->getRenderEngine();
        if (renderEngine == nullptr)
        {
            JUTILS_LOG(error, JSTR("Can't get render engine"));
            return false;
        }

        jarray<vertex_data> data = {
            { jutils::math::vector2{ 0.0f, 0.0f }, { 0.0f, 0.0f } },
            { jutils::math::vector2{ 0.0f, 1.0f }, { 0.0f, 1.0f } },
            { jutils::math::vector2{ 1.0f, 0.0f }, { 1.0f, 0.0f } },
            { jutils::math::vector2{ 1.0f, 0.0f }, { 1.0f, 0.0f } },
            { jutils::math::vector2{ 0.0f, 1.0f }, { 0.0f, 1.0f } },
            { jutils::math::vector2{ 1.0f, 1.0f }, { 1.0f, 1.0f } }
        };

        m_VertexBufferUI = renderEngine->createVertexBuffer(JumaRE::MakeVertexBufferData({ {
            meshesSubsystem->getVertexComponentID(VertexComponent::Position2D),
            meshesSubsystem->getVertexComponentID(VertexComponent::TextureCoords)
        } }, data));
        return m_VertexBufferUI != nullptr;
    }
    void UISubsystem::clearSubsystem()
    {
        if (m_VertexBufferUI != nullptr)
        {
            JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
            if (renderEngine != nullptr)
            {
                renderEngine->destroyVertexBuffer(m_VertexBufferUI);
            }
            m_VertexBufferUI = nullptr;
        }

        Super::clearSubsystem();
    }
}
