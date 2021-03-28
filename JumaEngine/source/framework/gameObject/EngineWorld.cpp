// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "EngineWorld.h"
#include "GameObject.h"
#include "gameComponent/SceneComponent.h"

namespace JumaEngine
{
    EngineWorld::~EngineWorld()
    {
        for (auto& Object : m_Objects)
        {
            delete Object;
        }
        m_Objects.clear();
        m_RootGameObject = nullptr;
    }

    void EngineWorld::onRegister()
    {
        m_RootGameObject = createObject<GameObject>();
        m_RootGameObject->m_RootComponent = createObject<SceneComponent>();
        m_RootGameObject->m_RootComponent->m_OwnerObject = m_RootGameObject;
    }

    void EngineWorld::registerWorldObject(WorldContextObject* object)
    {
        if (object != nullptr)
        {
            object->m_World = this;

            m_Objects.add(object);
            GameObject* gameObject = cast<GameObject>(object);
            if (gameObject != nullptr)
            {
                m_GameObjects.add(gameObject);
            }

            object->onRegisterInWorld();
        }
    }

    GameObject* EngineWorld::getRootGameObject(const WorldContextObject* object)
    {
        const EngineWorld* world = object != nullptr ? object->getOwnerWorld() : nullptr;
        return world != nullptr ? world->getRootGameObject() : nullptr;
    }
    bool EngineWorld::isRootGameObject(const GameObject* object)
    {
        return getRootGameObject(object) == object;
    }

    bool EngineWorld::isObjectsInSameWorld(const WorldContextObject* object1, const WorldContextObject* object2)
    {
        const EngineWorld* world1 = object1 != nullptr ? object1->getOwnerWorld() : nullptr;
        const EngineWorld* world2 = object2 != nullptr ? object2->getOwnerWorld() : nullptr;
        return (world1 != nullptr) && (world2 != nullptr) && (world1 == world2);
    }
    bool EngineWorld::isComponentUnderComponent(const SceneComponent* sceneComponent, const SceneComponent* parentComponent)
    {
        if (!isObjectsInSameWorld(sceneComponent, parentComponent))
        {
            return false;
        }

        while (sceneComponent->getParentComponent() != nullptr)
        {
            if (sceneComponent == parentComponent)
            {
                return true;
            }
            sceneComponent = sceneComponent->getParentComponent();
        }

        return isGameObjectUnderComponent(sceneComponent->getOwnerGameObject(), parentComponent);
    }
    bool EngineWorld::isGameObjectUnderComponent(const GameObject* object, const SceneComponent* parentComponent)
    {
        return object != nullptr ? isComponentUnderComponent(object->getParentComponent(), parentComponent) : false;
    }
    bool EngineWorld::isComponentUnderGameObject(const SceneComponent* sceneComponent, const GameObject* parentObject)
    {
        return parentObject != nullptr ? isComponentUnderComponent(sceneComponent, parentObject->getRootComponent()) : false;
    }
    bool EngineWorld::isGameObjectUnderGameObject(const GameObject* object, const GameObject* parentObject)
    {
        return parentObject != nullptr ? isGameObjectUnderComponent(object, parentObject->getRootComponent()) : false;
    }

    void EngineWorld::attachGameObject(GameObject* object, const GameObject* parentObject)
    {
        if (!isObjectsInSameWorld(object, parentObject))
        {
            // Incorrect data
            return;
        }
        if (isRootGameObject(object))
        {
            // Can't reattach root object
            return;
        }
        if (object->getParentGameObject() == parentObject)
        {
            // Already attached
            return;
        }

        attachGameObject(object, parentObject->getRootComponent());
    }
    void EngineWorld::attachGameObject(GameObject* object, SceneComponent* parentComponent)
    {
        if (!isObjectsInSameWorld(object, parentComponent))
        {
            // Incorrect data
            return;
        }
        if (isRootGameObject(object))
        {
            // Can't reattach root object
            return;
        }
        if (object->getParentComponent() == parentComponent)
        {
            // Already attached
            return;
        }
        if (isComponentUnderGameObject(parentComponent, object))
        {
            // parentComponent attached to object
            return;
        }

        object->m_ParentComponent = parentComponent;

        object->onParentComponentChanged();
    }
    void EngineWorld::detachGameObject(GameObject* object)
    {
        const EngineWorld* world = object != nullptr ? object->getOwnerWorld() : nullptr;
        if (world != nullptr)
        {
            attachGameObject(object, world->getRootGameObject());
        }
    }

    void EngineWorld::attachSceneComponent(SceneComponent* sceneComponent, const GameObject* parentObject)
    {
        if (!isObjectsInSameWorld(sceneComponent, parentObject))
        {
            // Incorrect data
            return;
        }
        if (sceneComponent->getOwnerGameObject() == parentObject)
        {
            // Already attached
            return;
        }

        attachSceneComponent(sceneComponent, parentObject->getRootComponent());
    }
    void EngineWorld::attachSceneComponent(SceneComponent* sceneComponent, SceneComponent* parentComponent)
    {
        if (!isObjectsInSameWorld(sceneComponent, parentComponent))
        {
            // Incorrect data
            return;
        }
        if (sceneComponent->isRootComponent())
        {
            // Can't reattach root component
            return;
        }
        SceneComponent* oldParentComponent = sceneComponent->getParentComponent();
        if (oldParentComponent == parentComponent)
        {
            // Already attached
            return;
        }
        if (isComponentUnderComponent(parentComponent, sceneComponent))
        {
            // parentComponent attached to sceneComponent
            return;
        }

        if (oldParentComponent != nullptr)
        {
            oldParentComponent->m_ChildComponents.remove(sceneComponent);
        }
        parentComponent->m_ChildComponents.add(sceneComponent);
        sceneComponent->m_ParentComponent = parentComponent;

        if ((oldParentComponent != nullptr) && (oldParentComponent->getOwnerGameObject() != parentComponent->getOwnerGameObject()))
        {
            sceneComponent->onOwnerGameObjectChange();
        }
        sceneComponent->onParentComponentChanged();
    }
    void EngineWorld::detachSceneComponent(SceneComponent* sceneComponent)
    {
        const EngineWorld* world = sceneComponent != nullptr ? sceneComponent->getOwnerWorld() : nullptr;
        if (world != nullptr)
        {
            attachSceneComponent(sceneComponent, world->getRootGameObject());
        }
    }

    void EngineWorld::attachGameComponent(GameComponent* component, GameObject* parentObject)
    {
        if (!isObjectsInSameWorld(component, parentObject))
        {
            // Incorrect data
            return;
        }

        SceneComponent* sceneComponent = cast<SceneComponent>(component);
        if (sceneComponent != nullptr)
        {
            // This is SceneComponent
            attachSceneComponent(sceneComponent, parentObject);
            return;
        }

        GameObject* oldParentObject = component->getOwnerGameObject();
        if (oldParentObject == parentObject)
        {
            // Already attached
            return;
        }

        if (oldParentObject != nullptr)
        {
            oldParentObject->m_Components.remove(component);
        }
        parentObject->m_Components.addUnique(component);
        component->m_OwnerObject = parentObject;

        component->onOwnerGameObjectChange();
    }
    void EngineWorld::detachGameComponent(GameComponent* component)
    {
        const EngineWorld* world = component != nullptr ? component->getOwnerWorld() : nullptr;
        if (world != nullptr)
        {
            attachGameComponent(component, world->getRootGameObject());
        }
    }

    void EngineWorld::draw()
    {
        for (auto& gameObject : m_GameObjects)
        {
            if (gameObject != nullptr)
            {
                gameObject->draw();
            }
        }
    }
}
