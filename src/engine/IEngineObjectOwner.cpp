// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/engine/IEngineObjectOwner.h"

#include "JumaEngine/engine/EngineObject.h"

namespace JumaEngine
{
    void IEngineObjectOwner::InitializeEngineObject(EngineObject* object)
    {
        object->initializeEngineObject();
    }
    void IEngineObjectOwner::ActivateEngineObject(EngineObject* object)
    {
        object->activateEngineObject();
    }
    void IEngineObjectOwner::UpdateEngineObject(EngineObject* object, const float deltaTime)
    {
        object->update(deltaTime);
    }
    void IEngineObjectOwner::PreRenderEngineObject(EngineObject* object)
    {
        object->preRender();
    }
    void IEngineObjectOwner::DeactivateEngineObject(EngineObject* object)
    {
        object->deactivateEngineObject();
    }
    void IEngineObjectOwner::ClearEngineObject(EngineObject* object)
    {
        object->clearEngineObject();
    }
}
