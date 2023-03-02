﻿// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"

#include <JumaRE/RenderEngine.h>

#include "EngineSubsystem.h"
#include "../game/GameInstance.h"

namespace JumaEngine
{
    class RenderTarget;
    class WidgetContext;

	class Engine : public EngineObjectOwner
    {
        friend EngineObject;

    protected:
        Engine() = default;
    public:
        virtual ~Engine() override = default;

        EngineObjectPtr<EngineContextObject> createObject(const EngineClass* objectClass) { return createObjectDescriptor(objectClass); }
        template<typename T>
        EngineObjectPtr<T> createObject(const EngineSubclass<T>& objectClass) { return this->createObjectDescriptor(objectClass.get()); }
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<EngineContextObject, T>)>
        EngineObjectPtr<T> createObject() { return this->createObject<T>(T::GetClassStatic()); }
        
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<GameInstance, T>)>
        bool init() { return this->init(T::GetClassStatic()); }
        void start();

        GameInstance* getGameInstance() const { return m_GameInstance.get(); }
        template<typename T, TEMPLATE_ENABLE(is_base<GameInstance, T>)>
        T* getGameInstance() const { return dynamic_cast<T*>(this->getGameInstance()); }

        JumaRE::RenderEngine* getRenderEngine() const { return m_RenderEngine; }

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<EngineSubsystem, T>)>
        T* createSubsystem() { return dynamic_cast<T*>(this->createSubsystem(T::GetClassStatic())); }
        template<typename T, TEMPLATE_ENABLE(is_base<EngineSubsystem, T>)>
        T* getSubsystem() const { return dynamic_cast<T*>(this->getSubsystem(T::GetClassStatic())); }
        WidgetsCreator* getWidgetsCreator() const { return m_EngineWidgetCreator.get(); }

        const jstring& getEngineContentDirectory() const { return m_EngineContentDirectory; }
        const jstring& getGameContentDirectory() const { return m_GameContentDirectory; }

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
        jarray<jdescriptor_table<EngineContextObject>::weak_pointer> m_DestroyingEngineObjects;

        EngineObjectPtr<GameInstance> m_GameInstance = nullptr;
        JumaRE::RenderEngine* m_RenderEngine = nullptr;

        jmap<EngineSubclass<EngineSubsystem>, EngineObjectPtr<EngineSubsystem>> m_EngineSubsystems;
        EngineObjectPtr<WidgetsCreator> m_EngineWidgetCreator = nullptr;
        
        jstring m_EngineContentDirectory = JSTR("./content_engine/");
        jstring m_GameContentDirectory = JSTR("./content/");


        jdescriptor_table<EngineContextObject>::pointer createObjectDescriptor(const EngineClass* objectClass);
        void onEngineObjectDestroying(EngineObject* object);
        void onEngineObjectDescriptorDestroying(EngineContextObject* object);

        bool init(const EngineSubclass<GameInstance>& gameInstanceClass);

        void clear();

        EngineSubsystem* createSubsystem(const EngineSubclass<EngineSubsystem>& subsystemClass);
        EngineSubsystem* getSubsystem(const EngineSubclass<EngineSubsystem>& subsystemClass) const;
    };
}
