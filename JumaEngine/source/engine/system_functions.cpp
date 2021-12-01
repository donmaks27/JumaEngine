// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "system_functions.h"
#include "render/RenderManagerBase.h"

namespace JumaEngine
{
    CameraComponent* System::getWindowActiveCamera(const EngineContextObjectOld* EngineContextObjectOld, const window_id windowID)
    {
        const RenderManagerBase* renderManager = EngineContextObjectOld != nullptr ? EngineContextObjectOld->getRenderManager() : nullptr;
        return renderManager != nullptr ? renderManager->getWindowActiveCamera(windowID) : nullptr;
    }
}
