﻿// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "engine/EngineContextObject.h"

namespace JumaEngine
{
    class SubsystemBase : public EngineContextObject
    {
        JUMAENGINE_CLASS(SubsystemBase, EngineContextObject)

    public:
        SubsystemBase() = default;
        virtual ~SubsystemBase() override = default;

        bool isInit() const { return m_Initialized && !m_Terminated; }
        void terminate();

    protected:

        virtual void onRegister() override;

        virtual bool initSubsystem() { return true; }
        virtual void terminateSubsystem() {}

    private:

        bool m_Initialized = false;
        bool m_Terminated = false;
    };
}
