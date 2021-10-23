// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "SubsystemBase.h"

namespace JumaEngine
{
    void SubsystemBase::onRegister()
    {
        Super::onRegister();

        if (!initSubsystem())
        {
            throw std::runtime_error("Failed to initialize subsystem " + getClass()->getClassName());
        }
        m_Initialized = true;
    }

    void SubsystemBase::terminate()
    {
        if (isInit())
        {
            terminateSubsystem();
            m_Terminated = true;
        }
    }
}
