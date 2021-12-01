// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "engine/EngineContextObject.h"

namespace JumaEngine
{
    class SubsystemBase : public EngineContextObject
    {
        JUMAENGINE_ABSTRACT_CLASS(SubsystemBase, EngineContextObject)

        friend Engine;

    public:
        SubsystemBase() = default;
        virtual ~SubsystemBase() override = default;

        bool isInit() const { return m_Initialized && !m_Terminated; }
        void clear();

    protected:

        virtual bool initSubsystemInternal() { return true; }
        virtual void clearSubsystemInternal() {}

    private:

        bool m_Initialized = false;
        bool m_Terminated = false;


        void initSubsystem();
    };
}
