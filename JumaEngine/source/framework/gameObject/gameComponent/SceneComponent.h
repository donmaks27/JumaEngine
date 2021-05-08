// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "GameComponent.h"
#include "framework/gameObject/TransformObject.h"
#include "render/IRenderInterface.h"
#include "utils/jarray.h"

namespace JumaEngine
{
    class SceneComponent : public GameComponent, public TransformObject, public IRenderInterface
    {
        JUMAENGINE_CLASS(SceneComponent, GameComponent)

        friend EngineWorld;

    public:
        SceneComponent() = default;
        virtual ~SceneComponent() override = default;

        SceneComponent* getParentComponent() const { return m_ParentComponent; }
        const jarray<SceneComponent*>& getChildComponents() const { return m_ChildComponents; }

        bool isRootComponent() const { return (getParentComponent() == nullptr) && (getOwnerGameObject() != nullptr); }

        virtual void render(window_id windowID) override;

    protected:

        virtual void onParentComponentChanged() {}

    private:

        SceneComponent* m_ParentComponent = nullptr;
        jarray<SceneComponent*> m_ChildComponents;
    };
}
