// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/engine/EngineObjectOwner.h"

#include "JumaEngine/engine/EngineObject.h"

namespace JumaEngine
{
    void EngineObjectOwner::InitializeEngineObject(EngineObject* object)
    {
        object->initializeEngineObject();
    }
    void EngineObjectOwner::ActivateEngineObject(EngineObject* object)
    {
        object->activateEngineObject();
    }
    void EngineObjectOwner::UpdateEngineObject(EngineObject* object, const float deltaTime)
    {
        object->update(deltaTime);
    }
    void EngineObjectOwner::PreRenderEngineObject(EngineObject* object)
    {
        object->preRender();
    }
    void EngineObjectOwner::DeactivateEngineObject(EngineObject* object)
    {
        object->deactivateEngineObject();
    }
    void EngineObjectOwner::ClearEngineObject(EngineObject* object)
    {
        object->clearEngineObject();
    }
}
