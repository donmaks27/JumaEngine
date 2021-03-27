// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "GameComponent.h"
#include "framework/gameObject/TransformObject.h"
#include "utils/jarray.h"

namespace JumaEngine
{
    class SceneComponent : public GameComponent, public TransformObject
    {
    public:
        SceneComponent() = default;
        virtual ~SceneComponent() override = default;

        SceneComponent* getParentComponent() const { return m_ParentComponent; }
        const jarray<SceneComponent*>& getChildComponents() const { return m_ChildComponents; }

    protected:

        virtual TransformObject* getParentTransformObject() const override;

    private:

        SceneComponent* m_ParentComponent = nullptr;
        jarray<SceneComponent*> m_ChildComponents;
    };
}
