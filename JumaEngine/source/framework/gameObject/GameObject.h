// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "WorldContextObject.h"
#include "TransformObject.h"
#include "utils/jarray.h"

namespace JumaEngine
{
    class GameComponent;
    class SceneComponent;

	class GameObject : public WorldContextObject, public TransformObject
	{
        friend EngineWorld;

	public:
		GameObject() = default;
		virtual ~GameObject() override = default;

        SceneComponent* getRootComponent() const { return m_RootComponent; }

        SceneComponent* getParentComponent() const { return m_ParentComponent; }
        GameObject* getParentGameObject() const;

        virtual void draw();

	protected:

        virtual void onParentComponentChanged() {}

	private:

        jarray<GameComponent*> m_Components;
        SceneComponent* m_RootComponent = nullptr;

        SceneComponent* m_ParentComponent = nullptr;
	};
}
