// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "engine/Engine.h"
#include "WorldContextObject.h"
#include "utils/jarray.h"
#include "engine/EngineContextObject.h"
#include "render/IRenderInterface.h"

namespace JumaEngine
{
    class GameObject;
    class GameComponent;
    class SceneComponent;

    class EngineWorld : public EngineContextObject, public IRenderInterface
    {
        JUMAENGINE_CLASS(EngineWorld, EngineContextObject)

    public:
        EngineWorld() = default;
        virtual ~EngineWorld() override;

        template<typename T, TEMPLATE_ENABLE(std::is_base_of_v<GameObject, T>)>
        T* createGameObject(SceneComponent* parentComponent = nullptr)
        {
        	if ((m_RootGameObject != nullptr) || !isGameStarted())
        	{
        		T* object = createObject<T>();
        		initializeGameObject(object, parentComponent);
        		return object;
        	}
        	return nullptr;
        }
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<SceneComponent, T>)>
        T* createSceneComponent(SceneComponent* parentComponent = nullptr)
        {
        	if (m_RootGameObject != nullptr)
        	{
        		T* component = createObject<T>();
        		initializeSceneComponent(component, parentComponent);
        		return component;
        	}
        	return nullptr;
        }
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<GameComponent, T> && !std::is_base_of_v<SceneComponent, T>)>
        T* createGameComponent(GameObject* parentObject = nullptr)
        {
        	if (m_RootGameObject != nullptr)
        	{
        		T* component = createObject<T>();
        		initializeGameComponent(component, parentObject);
        		return component;
        	}
        	return nullptr;
        }

    	void terminate();

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

    	void onGameStarted();
    	bool isGameStarted() const { return m_GameStarted; }

    	void tick(double deltaTime);

    	virtual void render(window_id windowID, const RenderParams& renderParams) override;

    protected:

        virtual void onRegister() override;

    private:

        jarray<WorldContextObject*> m_Objects;

        GameObject* m_RootGameObject = nullptr;
        jarray<GameObject*> m_GameObjects;

    	bool m_GameStarted = false;

        
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
    	
        void initializeGameObject(GameObject* object, SceneComponent* parentComponent);
        void initializeSceneComponent(SceneComponent* component, SceneComponent* parentComponent);
        void initializeGameComponent(GameComponent* component, GameObject* parentObject);
    };
}
