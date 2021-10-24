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

		static EngineOld* getEngine(const EngineContextObjectOld* EngineContextObjectOld) { return EngineContextObjectOld != nullptr ? EngineContextObjectOld->getOwnerEngine() : nullptr; }

        static CameraComponent* getWindowActiveCamera(const EngineContextObjectOld* EngineContextObjectOld, window_id windowID);
    };
}
