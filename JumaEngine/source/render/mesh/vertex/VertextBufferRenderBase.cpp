// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VertextBufferRenderBase.h"

#include "VertexBufferData.h"

namespace JumaEngine
{
    void VertextBufferRenderBase::init(const VertexBufferDataBase* vertexBufferData)
    {
        if (!isInit() && (vertexBufferData != nullptr) && (vertexBufferData->getVerticesCount() > 0))
        {
            m_VertexBufferData = vertexBufferData;
            if (!initInternal())
            {
                m_VertexBufferData = nullptr;
            }
        }
    }
    void VertextBufferRenderBase::terminate()
    {
        if (isInit())
        {
            terminateInternal();
            m_VertexBufferData = nullptr;
        }
    }
}
