﻿// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VertexBuffer.h"
#include "asset/mesh/VertexBufferData.h"
#include "utils/jlog.h"

namespace JumaEngine
{
    bool VertexBuffer::init(const VertexBufferDataBase* data)
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

        data->fillVertexBufferDescription(m_Description);
        if (!initInternal(data))
        {
            return false;
        }

        m_Initialized = true;
        return true;
    }

    void VertexBuffer::clear()
    {
        if (isValid())
        {
            clearInternal();
            m_Initialized = false;
        }
    }
}