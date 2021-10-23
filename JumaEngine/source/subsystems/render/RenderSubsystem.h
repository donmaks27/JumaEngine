// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "subsystems/SubsystemBase.h"
#include "subsystems/render/RenderInterface.h"
#include "subsystems/render/window/WindowDescription.h"

namespace JumaEngine
{
    class RenderSubsystem : public SubsystemBase, public IRenderInterface
    {
        JUMAENGINE_CLASS(RenderSubsystem, SubsystemBase)

    public:
        RenderSubsystem() = default;
        virtual ~RenderSubsystem() override = default;

    protected:

        void createMainWindow();
        void terminateMainWindow();
        virtual WindowDescription* createWindowInternal(const glm::uvec2& size, const jstring& title) = 0;
        virtual void terminateWindowInternal(WindowDescription* windowDescription) = 0;

    private:

        WindowDescription* m_MainWindow = nullptr;
    };
}
