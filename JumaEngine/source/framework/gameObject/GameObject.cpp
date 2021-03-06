// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "GameObject.h"
#include "gameComponent/SceneComponent.h"

namespace JumaEngine
{
    GameObject* GameObject::getParentGameObject() const
    {
        return getParentComponent() != nullptr ? getParentComponent()->getOwnerGameObject() : nullptr;
    }

    void GameObject::render(const window_id windowID, const RenderParams& renderParams)
    {
        if (m_RootComponent != nullptr)
        {
            m_RootComponent->render(windowID, renderParams);
        }
    }
}
