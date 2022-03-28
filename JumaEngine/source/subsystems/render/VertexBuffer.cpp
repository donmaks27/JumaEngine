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

        markAsInitialized();
        return true;
    }

    VertexBufferRenderAPIObject* VertexBuffer::createRenderAPIObjectInternal()
    {
        return getOwnerEngine()->getRenderSubsystem()->createVertexBufferObject();
    }

    void VertexBuffer::clearData()
    {
        clearRenderAPIObject();
        if (m_VertexData != nullptr)
        {
            delete m_VertexData;
            m_VertexData = nullptr;
        }

        m_VertexCount = 0;
        m_IndexCount = 0;
        m_VertexName = jstringID_NONE;
    }

    bool VertexBuffer::render(const RenderOptions* renderOptions) const
    {
        VertexBufferRenderAPIObject* renderObject = getRenderAPIObject();
        if (renderObject == nullptr)
        {
            return false;
        }
        return renderObject->render(renderOptions);
    }
}
