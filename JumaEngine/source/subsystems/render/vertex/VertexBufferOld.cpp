// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VertexBufferOld.h"
#include "VertexBufferData.h"
#include "jutils/jlog.h"

namespace JumaEngine
{
    bool VertexBufferOld::init(const VertexBufferDataBase* data)
    {
        if (isValid())
        {
            JUMA_LOG(warning, JSTR("Vertex buffer already initialized"));
            return false;
        }
        if (data == nullptr)
        {
            JUMA_LOG(warning, JSTR("Data is null"));
            return false;
        }

        m_VertexCount = data->getVertexCount();
        m_IndexCount = data->getIndexCount();
        m_VertexSize = data->getVertexSize();
        m_VertexComponents = data->getVertexComponents();
        if (!initInternal(data))
        {
            return false;
        }

        m_Initialized = true;
        return true;
    }

    void VertexBufferOld::clear()
    {
        if (isValid())
        {
            clearInternal();
            m_Initialized = false;
        }
    }
}
