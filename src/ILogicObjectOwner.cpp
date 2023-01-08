// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/ILogicObjectOwner.h"

#include "JumaEngine/LogicObject.h"

namespace JumaEngine
{
    void ILogicObjectOwner::InitializeLogicObject(LogicObject* object)
    {
        object->initializeLogicObject();
    }
    void ILogicObjectOwner::StartLogicObject(LogicObject* object)
    {
        object->startLogic();
    }
    void ILogicObjectOwner::UpdateLogicObject(LogicObject* object, const float deltaTime)
    {
        object->update(deltaTime);
    }
    void ILogicObjectOwner::PostUpdateLogicObject(LogicObject* object)
    {
        object->postUpdate();
    }
    void ILogicObjectOwner::StopLogicObject(LogicObject* object)
    {
        object->stopLogic();
    }
    void ILogicObjectOwner::DestroyLogicObject(LogicObject* object)
    {
        object->destroyLogicObject();
    }
}
