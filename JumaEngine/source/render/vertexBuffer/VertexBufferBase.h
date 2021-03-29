// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

namespace JumaEngine
{
    class VertexBufferDataBase;

    class VertexBufferBase
    {
    public:
        VertexBufferBase() = default;
        virtual ~VertexBufferBase() = default;

        virtual void init(VertexBufferDataBase* vertexBufferData) = 0;
        virtual bool isInit() const = 0;
        virtual void terminate() = 0;

        virtual void render() = 0;
    };
}
