// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "MaterialBase.h"

namespace JumaEngine
{
    class MaterialSimple final : public MaterialBase
    {
    public:
        MaterialSimple() = default;
        virtual ~MaterialSimple() override = default;

    protected:

        virtual void loadShaderParams() const override {}
    };
}
