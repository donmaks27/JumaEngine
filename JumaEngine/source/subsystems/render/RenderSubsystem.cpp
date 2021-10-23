// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderSubsystem.h"

namespace JumaEngine
{
    bool RenderSubsystem::initSubsystem()
    {
        if (!Super::initSubsystem())
        {
            return false;
        }

        return true;
    }

    void RenderSubsystem::terminateSubsystem()
    {
        Super::terminateSubsystem();
    }
}
