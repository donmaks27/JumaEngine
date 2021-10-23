// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "system_functions.h"
#include "render/RenderManagerBase.h"

namespace JumaEngine
{
    CameraComponent* System::getWindowActiveCamera(const EngineContextObject* engineContextObject, const window_id windowID)
    {
        const RenderManagerBase* renderManager = engineContextObject != nullptr ? engineContextObject->getRenderManager() : nullptr;
        return renderManager != nullptr ? renderManager->getWindowActiveCamera(windowID) : nullptr;
    }
}
