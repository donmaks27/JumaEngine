// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "utils/type_checks.h"

namespace JumaEngine
{
    struct RenderOptionsData
    {
        bool invertFacesOrientation = false;
    };

    struct RenderOptions
    {
        RenderOptionsData* data = nullptr;

        RenderOptionsData* getData() const { return data; }
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_same<RenderOptionsData, T>)>
        T* getData() const { return static_cast<T*>(data); }
    };
}
