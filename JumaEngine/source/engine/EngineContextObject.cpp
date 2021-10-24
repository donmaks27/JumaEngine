// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "EngineContextObject.h"
#include "Engine.h"

namespace JumaEngine
{
    RenderManagerBase* EngineContextObjectOld::getRenderManager() const
    {
        return m_OwnerEngine != nullptr ? m_OwnerEngine->getRenderManager() : nullptr;
    }

    bool EngineContextObjectOld::isMainThread() const
    {
        return (m_OwnerEngine != nullptr) && m_OwnerEngine->isMainThread();
    }
}
