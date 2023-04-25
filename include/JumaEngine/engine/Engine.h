// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "EngineObjectOwner.h"

#include <JumaRE/RenderEngine.h>

#include "EngineSubsystem.h"

namespace JumaEngine
{
	class GameInstance;
    class RenderTarget;
	class WidgetsCreator;

	class Engine : public EngineObjectOwner
    {
        friend EngineObject;
        friend EngineObjectPtrBase;

    protected:
        Engine() = default;
    public:
        virtual ~Engine() override = default;

        EngineObjectPtr<EngineContextObject> createObject(const EngineClass* objectClass) { return createObjectDescriptor(objectClass); }
        template<typename T>
        EngineObjectPtr<T> createObject(const EngineSubclass<T>& objectClass) { return this->createObjectDescriptor(objectClass.get()); }
        template<typename T> requires is_base_and_not_abstract_class<EngineContextObject, T>
        EngineObjectPtr<T> createObject() { return this->createObject<T>(T::GetClassStatic()); }
        
        template<typename T> requires is_base_and_not_abstract_class<GameInstance, T>
        bool init() { return this->init(T::GetClassStatic()); }
        void start();

        GameInstance* getGameInstance() const { return m_GameInstance.get(); }
        template<typename T> requires is_base_class<GameInstance, T>
        T* getGameInstance() const { return dynamic_cast<T*>(this->getGameInstance()); }

        JumaRE::RenderEngine* getRenderEngine() const { return m_RenderEngine; }

        template<typename T> requires is_base_and_not_abstract_class<EngineSubsystem, T>
        T* createSubsystem() { return dynamic_cast<T*>(this->createSubsystem(T::GetClassStatic())); }
        template<typename T> requires is_base_class<EngineSubsystem, T>
        T* getSubsystem() const { return dynamic_cast<T*>(this->getSubsystem(T::GetClassStatic())); }
        WidgetsCreator* getWidgetsCreator() const { return m_EngineWidgetCreator.get(); }
        
    protected:
        
        virtual bool initEngine();
        virtual bool initRenderEngine();

        virtual JumaRE::RenderAPI getDesiredRenderAPI() const { return JumaRE::RenderAPI::Vulkan; }
        virtual jstring getWindowsTitle() const { return JSTR("JumaEngine"); }
        virtual EngineObjectPtr<RenderTarget> getGameInstanceRenderTarget() const { return nullptr; }

        virtual bool onEngineLoopStarting();
        virtual void onEngineLoopStarted();
        virtual bool shouldStopEngineLoop();
        virtual void update(float deltaTime);
        virtual void preRender();
        virtual void onEngineLoopStopped();

        virtual void clearRenderEngine();
        virtual void clearEngine();

        void passInputToGameInstance(const JumaRE::InputActionData& input);

    private:

        jdescriptor_table<EngineContextObject> m_EngineObjectDescriptors;
        jarray<jdescriptor_table_pointer> m_DestroyingEngineObjects;

        EngineObjectPtr<GameInstance> m_GameInstance = nullptr;
        JumaRE::RenderEngine* m_RenderEngine = nullptr;

        jmap<EngineSubclass<EngineSubsystem>, EngineObjectPtr<EngineSubsystem>> m_EngineSubsystems;
        EngineObjectPtr<WidgetsCreator> m_EngineWidgetCreator = nullptr;


        jdescriptor_table_pointer createObjectDescriptor(const EngineClass* objectClass);
        void onEngineObjectDescriptorDestroying(EngineContextObject* object);
        void onEngineObjectDestroying(EngineObject* object);

        bool init(const EngineSubclass<GameInstance>& gameInstanceClass);

        void clear();

        EngineSubsystem* createSubsystem(const EngineSubclass<EngineSubsystem>& subsystemClass);
        EngineSubsystem* getSubsystem(const EngineSubclass<EngineSubsystem>& subsystemClass) const;
    };
}
