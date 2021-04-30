// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "VertexBufferDescription.h"

namespace JumaEngine
{
    class VertexBufferDataBase;

    class VertexBufferBase
    {
    public:
        VertexBufferBase() = default;
        virtual ~VertexBufferBase() = default;

        bool init(VertexBufferDataBase* vertexBufferData);
        bool isInit() const { return m_Initialized; }
        void terminate();

        const VertexBufferDescription& getVertexBufferDescription() const { return m_VertexBufferDescription; }

        virtual void render() = 0;

    protected:

        virtual bool initInternal(VertexBufferDataBase* vertexBufferData) = 0;
        virtual void terminateInteranl() = 0;

    private:

        bool m_Initialized = false;
        VertexBufferDescription m_VertexBufferDescription = VertexBufferDescription();
    };
}
