// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "engine/Engine.h"
#include "render/window/window_id.h"

namespace JumaEngine
{
    class MeshFileImporterBase;
    class CameraComponent;

    class System final
    {
    public:

		static Engine* getEngine(const EngineContextObject* engineContextObject) { return engineContextObject != nullptr ? engineContextObject->getOwnerEngine() : nullptr; }

        static CameraComponent* getWindowActiveCamera(const EngineContextObject* engineContextObject, window_id windowID);
    };
}
