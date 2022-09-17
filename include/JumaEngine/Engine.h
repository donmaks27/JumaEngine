// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "core.h"

#include <JumaRE/RenderEngine.h>

namespace JumaEngine
{
    class GameInstance;
    class EngineContextObject;

    class Engine
    {
    public:
        Engine() = default;
        virtual ~Engine();

        bool init();
        void clear();
        
        JumaRE::RenderEngine* getRenderEngine() const { return m_RenderEngine; }
        
        GameInstance* getGameInstance() const { return m_GameInstance; }
        template<typename T, TEMPLATE_ENABLE(is_base<GameInstance, T>)>
        T* getGameInstance() const { return dynamic_cast<T*>(getGameInstance()); }
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<GameInstance, T>)>
        bool createGameInstance()
        {
            if (m_GameInstance == nullptr)
            {
                m_GameInstance = createObject<T>();
                if (m_GameInstance != nullptr)
                {
                    return initGameInstance();
                }
            }
            return false;
        }
        void destroyGameInstance();

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<EngineContextObject, T>)>
        T* createObject() { return this->registerObject(new T()); }
        template<typename T, TEMPLATE_ENABLE(is_base<EngineContextObject, T>)>
        T* registerObject(T* object)
        {
            this->registerObjectInternal(object);
            return object;
        }
        
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


        void clearData_Engine();

        void registerObjectInternal(EngineContextObject* object);
        
        void onInputButton(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, 
            JumaRE::InputDevice device, JumaRE::InputButton button, JumaRE::InputButtonAction action);
        void onInputAxis(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, 
            JumaRE::InputDevice device, JumaRE::InputAxis axis, float value);
        void onInputAxis2D(JumaRE::WindowController* windowController, const JumaRE::WindowData* windowData, 
            JumaRE::InputDevice device, JumaRE::InputAxis axis, const math::vector2& value);
    };
}
