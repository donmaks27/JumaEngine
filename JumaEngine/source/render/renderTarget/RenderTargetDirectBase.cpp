// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderTargetDirectBase.h"
#include "framework/gameObject/EngineWorld.h"
#include "framework/gameObject/gameComponent/CameraComponent.h"
#include "render/RenderManagerBase.h"
#include "utils/system_functions.h"

namespace JumaEngine
{
    void RenderTargetDirectBase::render(const window_id windowID)
    {
        EngineWorld* world = m_Camera != nullptr ? m_Camera->getOwnerWorld() : nullptr;
        if (world != nullptr)
        {
            RenderManagerBase* renderManager = SystemFunctions::getRenderManager(this);
            if ((renderManager != nullptr) && renderManager->isValidWindowID(windowID))
            {
                renderManager->setWindowActiveCamera(windowID, m_Camera);

                clearFrame();
                world->render(windowID);
            }
        }
    }
}
