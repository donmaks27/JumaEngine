// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VertexBufferBase.h"
#include "VertexBufferData.h"

namespace JumaEngine
{
    bool VertexBufferBase::init(VertexBufferDataBase* vertexBufferData)
    {
        if (!isInit() && (vertexBufferData != nullptr))
        {
            vertexBufferData->fillVertexBufferDescription(m_VertexBufferDescription);
            m_Initialized = initInternal(vertexBufferData);
            return isInit();
        }
        return false;
    }

    void VertexBufferBase::terminate()
    {
        if (isInit())
        {
            terminateInteranl();
            m_Initialized = false;
        }
    }
}
