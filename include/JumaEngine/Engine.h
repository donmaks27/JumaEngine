// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"

#include <JumaRE/RenderEngine.h>

#include "subsystems/EngineSubsystem.h"

namespace JumaEngine
{
    class GameInstance;

    class Engine
    {
    public:
        Engine() = default;
        virtual ~Engine();

        bool init();
        void clear();
        
        JumaRE::RenderEngine* getRenderEngine() const { return m_RenderEngine; }

        template<typename T, TEMPLATE_ENABLE(is_base<EngineContextObject, T>)>
        T* registerObject(T* object) { return dynamic_cast<T*>(this->registerObjectInternal(object)); }

        EngineContextObject* createObject(const EngineClass* engineClass);
        template<typename T>
        T* createObject(const EngineSubclass<T>& subclass) { return dynamic_cast<T*>(this->createObject(subclass.get())); }
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<EngineContextObject, T>)>
        T* createObject() { return this->createObject<T>(T::GetClassStatic()); }
        
        GameInstance* getGameInstance() const { return m_GameInstance; }
        template<typename T, TEMPLATE_ENABLE(is_base<GameInstance, T>)>
        T* getGameInstance() const { return dynamic_cast<T*>(this->getGameInstance()); }
        GameInstance* createGameInstance(const EngineSubclass<GameInstance>& subclass);
        template<typename T, TEMPLATE_ENABLE(is_base<GameInstance, T>)>
        T* createGameInstance() { return dynamic_cast<T*>( this->createGameInstance(T::GetClassStatic()) ); }
        void destroyGameInstance();

        EngineSubsystem* getSubsystem(const EngineSubclass<EngineSubsystem>& subclass);
        template<typename T, TEMPLATE_ENABLE(is_base<EngineSubsystem, T>)>
        T* getSubsystem() { return dynamic_cast<T*>( this->getSubsystem(T::GetClassStatic()) ); }
        
        virtual bool update() = 0;

    protected:

        virtual bool initInternal();
        virtual bool initGameInstance() = 0;
        virtual void clearInternal();

        bool createRenderEngine(JumaRE::RenderAPI api, const JumaRE::WindowCreateInfo& mainWindowInfo);
        void destroyRenderEngine();

    private:

        JumaRE::RenderEngine* m_RenderEngine = nullptr;
        GameInstance* m_GameInstance = nullptr;

        jmap<EngineSubclass<EngineSubsystem>, EngineSubsystem*> m_EngineSubsystems;


        void clearData_Engine();
        void destroyEngineSubsystems();

        EngineContextObject* registerObjectInternal(EngineContextObject* object);
        
        void onInputButton(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, 
            JumaRE::InputDevice device, JumaRE::InputButton button, JumaRE::InputButtonAction action);
        void onInputAxis(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, 
            JumaRE::InputDevice device, JumaRE::InputAxis axis, float value);
        void onInputAxis2D(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, 
            JumaRE::InputDevice device, JumaRE::InputAxis axis, const math::vector2& value);
    };
}
