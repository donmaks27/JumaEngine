// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderTargetBase.h"

namespace JumaEngine
{
    class CameraComponent;

    class RenderTargetDirectBase : public RenderTargetBase
    {
        JUMAENGINE_CLASS(RenderTargetDirectBase, RenderTargetBase)

    public:
        RenderTargetDirectBase() = default;
        virtual ~RenderTargetDirectBase() override = default;

        void setCamera(CameraComponent* camera) { m_Camera = camera; }

        virtual void render(window_id windowID) override;

    protected:

        virtual void clearFrame() = 0;

    private:

        CameraComponent* m_Camera = nullptr;
    };
}
