// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "SubsystemBase.h"

namespace JumaEngine
{
    void SubsystemBase::onRegister()
    {
        Super::onRegister();

        m_Initialized = initSubsystem();
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
