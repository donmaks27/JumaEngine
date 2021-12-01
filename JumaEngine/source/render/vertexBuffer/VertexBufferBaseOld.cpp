// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VertexBufferBaseOld.h"
#include "asset/mesh/VertexBufferData.h"

namespace JumaEngine
{
    bool VertexBufferBaseOld::init(VertexBufferDataBase* vertexBufferData)
    {
        if (!isInit() && (vertexBufferData != nullptr))
        {
            vertexBufferData->fillVertexBufferDescription(m_VertexBufferDescription);
            m_Initialized = initInternal(vertexBufferData);
            return isInit();
        }
        return false;
    }
}
