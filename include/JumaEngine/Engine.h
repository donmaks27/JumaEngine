// Copyright © 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"

#include <JumaRE/RenderEngine.h>

#include "GameInstance.h"
#include "subsystems/EngineSubsystem.h"

namespace JumaEngine
{
    class Engine
    {
    protected:
        Engine() = default;
    public:
        virtual ~Engine() = default;

        template<typename T, TEMPLATE_ENABLE(is_base<EngineContextObject, T>)>
        T* registerObject(T* object) { return dynamic_cast<T*>(this->registerObjectInternal(object)); }
        EngineContextObject* createObject(const EngineClass* engineClass);
        template<typename T>
        T* createObject(const EngineSubclass<T>& objectClass) { return dynamic_cast<T*>(this->createObject(objectClass.get())); }
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<EngineContextObject, T>)>
        T* createObject() { return this->createObject<T>(T::GetClassStatic()); }

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<GameInstance, T>)>
        bool init() { return this->init(T::GetClassStatic()); }
        void start();
        void clear();

        GameInstance* getGameInstance() const { return m_GameInstance; }
        template<typename T, TEMPLATE_ENABLE(is_base<GameInstance, T>)>
        T* getGameInstance() const { return dynamic_cast<T*>(this->getGameInstance()); }

        JumaRE::RenderEngine* getRenderEngine() const { return m_RenderEngine; }

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<EngineSubsystem, T>)>
        T* createSubsystem() { return dynamic_cast<T*>(this->createSubsystem(T::GetClassStatic())); }
        template<typename T, TEMPLATE_ENABLE(is_base<EngineSubsystem, T>)>
        T* getSubsystem() const { return dynamic_cast<T*>(this->getSubsystem(T::GetClassStatic())); }

    protected:

        JumaRE::WindowCreateInfo m_InitialRenderEngineWindow = { JSTR("JumaEngine"), { 800, 600 } };
        JumaRE::RenderTarget* m_InitialGameInstanceRenderTarger = nullptr;
        JumaRE::RenderAPI m_InitialRenderAPI = JumaRE::RenderAPI::OpenGL;


        virtual bool initEngine();
        virtual bool initGameInstance();
        virtual bool initRenderEngine();

        virtual bool initEngineLoop();
        virtual void startEngineLoop();
        virtual bool update();
        virtual void stopEngineLoop();

        virtual void clearRenderEngine();
        virtual void clearEngine();

    private:

        GameInstance* m_GameInstance = nullptr;
        JumaRE::RenderEngine* m_RenderEngine = nullptr;

        jmap<EngineSubclass<EngineSubsystem>, EngineSubsystem*> m_EngineSubsystems;


        EngineContextObject* registerObjectInternal(EngineContextObject* object);

        bool init(const EngineSubclass<GameInstance>& gameInstanceClass);

        EngineSubsystem* createSubsystem(const EngineSubclass<EngineSubsystem>& subsystemClass);
        EngineSubsystem* getSubsystem(const EngineSubclass<EngineSubsystem>& subsystemClass) const;
    };
}
