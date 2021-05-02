// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderTargetBase.h"

namespace JumaEngine
{
    class CameraComponent;

    class RenderTargetDirect : public RenderTargetBase
    {
    public:
        RenderTargetDirect() = default;
        virtual ~RenderTargetDirect() override = default;

        void setCamera(CameraComponent* camera) { m_Camera = camera; }

        virtual void render(window_id windowID) override;

    private:

        CameraComponent* m_Camera = nullptr;
    };
}
