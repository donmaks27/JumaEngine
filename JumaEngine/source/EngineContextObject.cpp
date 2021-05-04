// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "EngineContextObject.h"
#include "Engine.h"

namespace JumaEngine
{
    RenderManagerBase* EngineContextObject::getRenderManager() const
    {
        return m_OwnerEngine != nullptr ? m_OwnerEngine->getRenderManager() : nullptr;
    }

    bool EngineContextObject::isMainThread() const
    {
        return (m_OwnerEngine != nullptr) && m_OwnerEngine->isMainThread();
    }
}
