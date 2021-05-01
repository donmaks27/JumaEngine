// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderTargerBase.h"

namespace JumaEngine
{
    class CameraComponent;

    class RenderTarget : public RenderTargerBase
    {
    public:
        RenderTarget() = default;
        virtual ~RenderTarget() override = default;

    private:

        CameraComponent* m_Camera = nullptr;
    };
}
