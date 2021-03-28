// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "Engine.h"
#include "WorldContextObject.h"
#include "utils/jarray.h"

namespace JumaEngine
{
    class GameObject;
    class GameComponent;
    class SceneComponent;

    class EngineWorld : public EngineContextObject
    {
    public:
        EngineWorld() = default;
        virtual ~EngineWorld() override;

        template<typename T, TEMPLATE_ENABLE(std::is_base_of_v<GameObject, T>)>
        T* createGameObject(SceneComponent* parentComponent = nullptr)
        {
            T* object = createObject<T>();
            if (parentComponent != nullptr)
            {
                attachGameObject(object, parentComponent);
            }
            else
            {
                attachGameObject(object, getRootGameObject());
            }
            return object;
        }
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<SceneComponent, T>)>
        T* createSceneComponent(SceneComponent* parentComponent = nullptr)
        {
            T* component = createObject<T>();
            if (parentComponent != nullptr)
            {
                attachSceneComponent(component, parentComponent);
            }
            else
            {
                attachSceneComponent(component, getRootGameObject());
            }
            return component;
        }
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<GameComponent, T> && !std::is_base_of_v<SceneComponent, T>)>
        T* createGameComponent(GameObject* parentObject = nullptr)
        {
            T* component = createObject<T>();
            attachGameComponent(component, parentObject != nullptr ? parentObject : getRootGameObject());
            return component;
        }

        GameObject* getRootGameObject() const { return m_RootGameObject; }

        static GameObject* getRootGameObject(const WorldContextObject* object);
        static bool isRootGameObject(const GameObject* object);

        static bool isObjectsInSameWorld(const WorldContextObject* object1, const WorldContextObject* object2);
        static bool isComponentUnderComponent(const SceneComponent* sceneComponent, const SceneComponent* parentComponent);
        static bool isGameObjectUnderComponent(const GameObject* object, const SceneComponent* parentComponent);
        static bool isComponentUnderGameObject(const SceneComponent* sceneComponent, const GameObject* parentObject);
        static bool isGameObjectUnderGameObject(const GameObject* object, const GameObject* parentObject);

        static void attachGameObject(GameObject* object, const GameObject* parentObject);
        static void attachGameObject(GameObject* object, SceneComponent* parentComponent);
        static void detachGameObject(GameObject* object);

        static void attachSceneComponent(SceneComponent* sceneComponent, const GameObject* parentObject);
        static void attachSceneComponent(SceneComponent* sceneComponent, SceneComponent* parentComponent);
        static void detachSceneComponent(SceneComponent* sceneComponent);

        static void attachGameComponent(GameComponent* component, GameObject* parentObject);
        static void detachGameComponent(GameComponent* component);

        void draw();

    protected:

        virtual void onRegister() override;

    private:

        jarray<WorldContextObject*> m_Objects;

        GameObject* m_RootGameObject = nullptr;
        jarray<GameObject*> m_GameObjects;

        
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_same<WorldContextObject, T>)>
        T* createObject()
        {
            Engine* engine = getOwnerEngine();
            if (engine != nullptr)
            {
                T* object = engine->createObject<T>();
                registerWorldObject(object);
                return object;
            }
            return nullptr;
        }
        void registerWorldObject(WorldContextObject* object);
    };
}
