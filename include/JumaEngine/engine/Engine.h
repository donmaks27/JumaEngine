// Copyright © 2022-2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"

#include <JumaRE/RenderEngine.h>

#include "EngineSubsystem.h"
#include "../game/GameInstance.h"

namespace JumaEngine
{
	class WidgetContext;

	class Engine : public EngineObjectOwner
    {
    protected:
        Engine() = default;
    public:
        virtual ~Engine() override = default;

        EngineObjectPtr<EngineContextObject> createObject(const EngineClass* objectClass) { return createObjectDescriptor(objectClass); }
        template<typename T>
        EngineObjectPtr<T> createObject(const EngineSubclass<T>& objectClass) { return this->createObjectDescriptor(objectClass.get()); }
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<EngineContextObject, T>)>
        EngineObjectPtr<T> createObject() { return this->createObject<T>(T::GetClassStatic()); }
        template<typename T>
        void destroyObject(const EngineObjectPtr<T>& object) { this->destroyObject(object.updatePtr()); }
        template<typename T>
        void destroyObject(const EngineObjectWeakPtr<T>& object) { this->destroyObject(object.get()); }
        
        EngineContextObject* createObject1(const EngineClass* engineClass);
        template<typename T>
        T* createObject1(const EngineSubclass<T>& objectClass) { return dynamic_cast<T*>(this->createObject1(objectClass.get())); }
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<EngineContextObject, T>)>
        T* createObject1() { return this->createObject1<T>(T::GetClassStatic()); }

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<GameInstance, T>)>
        bool init() { return this->init(T::GetClassStatic()); }
        void start();

        GameInstance* getGameInstance() const { return m_GameInstance; }
        template<typename T, TEMPLATE_ENABLE(is_base<GameInstance, T>)>
        T* getGameInstance() const { return dynamic_cast<T*>(this->getGameInstance()); }

        JumaRE::RenderEngine* getRenderEngine() const { return m_RenderEngine; }

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<EngineSubsystem, T>)>
        T* createSubsystem() { return dynamic_cast<T*>(this->createSubsystem(T::GetClassStatic())); }
        template<typename T, TEMPLATE_ENABLE(is_base<EngineSubsystem, T>)>
        T* getSubsystem() const { return dynamic_cast<T*>(this->getSubsystem(T::GetClassStatic())); }
        WidgetsCreator* getWidgetsCreator() const { return m_EngineWidgetCreator; }

        const jstring& getEngineContentDirectory() const { return m_EngineContentDirectory; }
        const jstring& getGameContentDirectory() const { return m_GameContentDirectory; }

    protected:
        
        virtual bool initEngine();
        virtual bool initGameInstance();
        virtual bool initRenderEngine();

        virtual JumaRE::RenderAPI getDesiredRenderAPI() const { return JumaRE::RenderAPI::Vulkan; }
        virtual jstring getWindowsTitle() const { return JSTR("JumaEngine"); }
        virtual JumaRE::RenderTarget* getGameInstanceRenderTarget() const { return nullptr; }

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

        GameInstance* m_GameInstance = nullptr;
        JumaRE::RenderEngine* m_RenderEngine = nullptr;

        jmap<EngineSubclass<EngineSubsystem>, EngineSubsystem*> m_EngineSubsystems;
        WidgetsCreator* m_EngineWidgetCreator = nullptr;
        
        jstring m_EngineContentDirectory = JSTR("./content_engine/");
        jstring m_GameContentDirectory = JSTR("./content/");


        jdescriptor_table<EngineContextObject>::pointer createObjectDescriptor(const EngineClass* objectClass);
        void destroyObject(EngineContextObject* object);
        void onEngineObjectDestroying(EngineContextObject* object);
		EngineContextObject* registerObjectInternal1(EngineContextObject* object);

        bool init(const EngineSubclass<GameInstance>& gameInstanceClass);

        void clear();

        EngineSubsystem* createSubsystem(const EngineSubclass<EngineSubsystem>& subsystemClass);
        EngineSubsystem* getSubsystem(const EngineSubclass<EngineSubsystem>& subsystemClass) const;
    };
}
