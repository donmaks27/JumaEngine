// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "jutils/type_checks.h"

namespace JumaEngine
{
    struct RenderOptionsData
    {
        bool invertFacesOrientation = false;
    };

    struct RenderOptionsOld
    {
        RenderOptionsData* data = nullptr;

        RenderOptionsData* getData() const { return data; }
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_same<RenderOptionsData, T>)>
        T* getData() const { return static_cast<T*>(data); }
    };
}
