// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VertexBuffer.h"
#include "asset/mesh/VertexBufferData.h"

namespace JumaEngine
{
    bool VertexBuffer::init(const VertexBufferDataBase* data)
    {
        if (isValid() || (data == nullptr))
        {
            return false;
        }

        data->fillVertexBufferDescription(m_Description);
        if (!initInternal(data))
        {
            return false;
        }

        m_Initialized = true;
        return true;
    }
}
