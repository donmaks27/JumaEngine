// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "subsystems/SubsystemBase.h"

#include "Window.h"

namespace JumaEngine
{
    class WindowSubsystem : public SubsystemBase
    {
        JUMAENGINE_ABSTRACT_CLASS(WindowSubsystem, SubsystemBase)

    public:
        WindowSubsystem() = default;
        virtual ~WindowSubsystem() override = default;

        virtual Window* createWindow(const jstring& title, const math::uvector2& size = { 0, 0 }) = 0;
        virtual Window* findWindow(window_id_type windowID) const = 0;
        virtual void destroyWindow(window_id_type windowID) = 0;

    protected:

        juid<window_id_type> m_WindowIDs;
    };
}
