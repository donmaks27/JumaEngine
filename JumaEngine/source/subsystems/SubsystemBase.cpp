// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "SubsystemBase.h"
#include "jutils/jlog.h"

namespace JumaEngine
{
    void SubsystemBase::initSubsystem()
    {
        if (initSubsystemInternal())
        {
            m_Initialized = true;
        }
        else
        {
            JUMA_LOG(error, JSTR("Failed to initialize subsystem ") + getObjectClass()->getClassName());
        }
    }

    void SubsystemBase::clear()
    {
        if (isValid())
        {
            clearSubsystemInternal();
            m_Terminated = true;
        }
    }
}
