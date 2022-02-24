// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "VertexBuffer.h"

#include "RenderSubsystem.h"
#include "engine/Engine.h"
#include "vertex/VertexBufferData.h"

namespace JumaEngine
{
    VertexBuffer::~VertexBuffer()
    {
        clear();
    }

    bool VertexBuffer::init(const VertexBufferDataBase* data)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Vertex buffer already initialized"));
            return false;
        }
        if ((data == nullptr) || (data->getVertexCount() == 0))
        {
            JUMA_LOG(warning, JSTR("Vertex data is null"));
            return false;
        }

        m_VertexData = data;
        m_VertexName = data->getVertexName();
        m_VertexCount = data->getVertexCount();
        m_IndexCount = data->getIndexCount();

        m_Initialized = true;
        return true;
    }

    void VertexBuffer::clear()
    {
        if (isValid())
        {
            clearRenderObject();
            if (m_VertexData != nullptr)
            {
                delete m_VertexData;
                m_VertexData = nullptr;
            }
            m_VertexCount = 0;
            m_IndexCount = 0;
            m_VertexName = jstringID_NONE;
            m_Initialized = false;
        }
    }

    bool VertexBuffer::createRenderObject()
    {
        if (!isValid())
        {
            JUMA_LOG(warning, JSTR("Shader not initialized"));
            return false;
        }

        RenderSubsystem* renderSubsystem = getOwnerEngine()->getRenderSubsystem();
        m_RenderObject = renderSubsystem->createVertexBufferObject();
        if (!m_RenderObject->init(this))
        {
            delete m_RenderObject;
            m_RenderObject = nullptr;
            return false;
        }
        return true;
    }
    void VertexBuffer::clearRenderObject()
    {
        if (m_RenderObject != nullptr)
        {
            delete m_RenderObject;
            m_RenderObject = nullptr;
        }
    }

    bool VertexBuffer::render(const RenderOptions* renderOptions) const
    {
        if (m_RenderObject == nullptr)
        {
            return false;
        }
        return m_RenderObject->render(renderOptions);
    }
}
