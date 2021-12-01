// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#include "utils/jdelegate_multicast.h"
#include "utils/math/vector2.h"

namespace JumaEngine
{
    class WindowDescription;

    CREATE_JUTILS_MULTICAST_DELEGATE_OneParam(OnWindowDescriptionEvent, WindowDescription*, window);

    class WindowDescription
    {
    public:
        WindowDescription() = default;
        virtual ~WindowDescription() = default;

        OnWindowDescriptionEvent onSizeChanged;


        jstring title;
        math::uvector2 size = { 0, 0 };
    };
}
