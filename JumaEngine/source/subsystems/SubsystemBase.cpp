﻿// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "SubsystemBase.h"

namespace JumaEngine
{
    void SubsystemBase::initSubsystem()
    {
        if (!initSubsystemInternal())
        {
            throw std::runtime_error("Failed to initialize subsystem " + getClass()->getClassName());
        }
        m_Initialized = true;
    }

    void SubsystemBase::clear()
    {
        if (isInit())
        {
            clearSubsystemInternal();
            m_Terminated = true;
        }
    }
}
