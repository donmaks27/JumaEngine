// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "SceneComponent.h"

namespace JumaEngine
{
    void SceneComponent::render()
    {
        for (auto& component : m_ChildComponents)
        {
            if (component != nullptr)
            {
                component->render();
            }
        }
    }
}
