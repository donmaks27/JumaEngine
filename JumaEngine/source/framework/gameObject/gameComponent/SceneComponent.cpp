// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "SceneComponent.h"

namespace JumaEngine
{
    void SceneComponent::render(const window_id windowID, const RenderParams& renderParams)
    {
        for (auto& component : m_ChildComponents)
        {
            if (component != nullptr)
            {
                component->render(windowID, renderParams);
            }
        }
    }
}
