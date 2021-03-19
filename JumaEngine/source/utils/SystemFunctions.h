// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

namespace JumaEngine
{
    class EngineContextObject;
    class VertexBufferBase;
    class VertexBufferDataBase;

    class SystemFunctions
    {
    private:
        SystemFunctions() = default;

    public:

        static VertexBufferBase* createVertexBuffer(const EngineContextObject* engineContextObject, VertexBufferDataBase* vertexBufferData = nullptr);
    };
}
