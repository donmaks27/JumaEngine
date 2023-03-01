// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/engine/EngineContextObject.h"

#include "JumaEngine/engine/Engine.h"

namespace JumaEngine
{
    void EngineContextObject::destroy()
    {
        getEngine()->destroyObject(EngineObjectWeakPtr(this));
    }
}
