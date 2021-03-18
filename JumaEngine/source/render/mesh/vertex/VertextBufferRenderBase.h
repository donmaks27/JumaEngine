// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

namespace JumaEngine
{
    class VertexBufferDataBase;

    class VertextBufferRenderBase
    {
    public:
        VertextBufferRenderBase() = default;
        virtual ~VertextBufferRenderBase() = default;

        void init(const VertexBufferDataBase* vertexBufferData);
        bool isInit() const { return m_VertexBufferData != nullptr; }
        void terminate();

        virtual void draw() = 0;

    protected:

        const VertexBufferDataBase* m_VertexBufferData = nullptr;


        virtual bool initInternal() = 0;
        virtual void terminateInternal() = 0;
    };
}
