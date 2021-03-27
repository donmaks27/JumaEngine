// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "SceneComponent.h"
#include "framework/gameObject/SceneObject.h"

namespace JumaEngine
{
    TransformObject* SceneComponent::getParentTransformObject() const
    {
        if (m_ParentComponent != nullptr)
        {
            return m_ParentComponent;
        }

        SceneObject* ownerSceneObject = cast<SceneObject>(getOwnerObject());
        if (ownerSceneObject != nullptr)
        {
            return ownerSceneObject;
        }

        return nullptr;
    }
}
