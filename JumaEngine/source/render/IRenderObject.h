// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
//#include "window/window_id.h"

namespace JumaEngine
{
    class IRenderObject
    {
    protected:
        IRenderObject() = default;
        virtual ~IRenderObject() = default;

    public:

        virtual void render(/*window_id windowID*/) {}
    };
}
