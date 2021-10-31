// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include <glm/vec2.hpp>

#include "utils/jdelegate_multicast.h"
#include "utils/jshared_ptr.h"

namespace JumaEngine
{
    class WindowDescription;

    CREATE_JDELEGATE_MULTICAST_TYPE_OneParam(OnWindowDescriptionEvent, const jshared_ptr<WindowDescription>&, window)

    class WindowDescription
    {
    public:
        WindowDescription() = default;
        virtual ~WindowDescription() = default;

        OnWindowDescriptionEvent onSizeChanged;


        jstring title;
        glm::uvec2 size = { 0, 0 };
    };
}
