// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/engine/IEngineObjectOwner.h"

#include "JumaEngine/engine/EngineObject.h"

namespace JumaEngine
{
    void IEngineObjectOwner::InitializeLogicObject(EngineObject* object)
    {
        object->initializeLogicObject();
    }
    void IEngineObjectOwner::StartLogicObject(EngineObject* object)
    {
        object->startLogic();
    }
    void IEngineObjectOwner::UpdateLogicObject(EngineObject* object, const float deltaTime)
    {
        object->update(deltaTime);
    }
    void IEngineObjectOwner::PreRenderLogicObject(EngineObject* object)
    {
        object->preRender();
    }
    void IEngineObjectOwner::StopLogicObject(EngineObject* object)
    {
        object->stopLogic();
    }
    void IEngineObjectOwner::DestroyLogicObject(EngineObject* object)
    {
        object->destroyLogicObject();
    }
}
