// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderAPI.h"
#include "WindowDescriptionBase.h"
#include "utils/jmap_auto_id.h"
#include "EngineContextObject.h"

namespace JumaEngine
{
    class ShaderBase;
    class VertexBufferBase;

    typedef uint8 window_id_type;
    constexpr window_id_type INVALID_WINDOW_ID = 0;

    class RenderManager : public EngineContextObject
    {
    public:
        RenderManager() = default;
        virtual ~RenderManager() override;

        virtual RenderAPI getRenderAPI() const = 0;

        bool init();
        bool isInit() const { return m_Initialized && !m_Terminated; }
        void terminate();

        window_id_type getMainWindowID() const { return m_MainWindowID; }
        jarray<window_id_type> getWindowsList() const;

        bool getWindowSize(window_id_type windowID, glm::uvec2& outWindowSize) const;
        bool setWindowSize(window_id_type windowID, const glm::uvec2& size);

        bool getWindowTitle(window_id_type windowID, jstring& outWindowTitle) const;
        bool setWindowTitle(window_id_type windowID, const jstring& title);
        
        virtual ShaderBase* createShader() = 0;
        virtual VertexBufferBase* createVertextBuffer() = 0;

        virtual void startRender() = 0;
        virtual void finishRender() = 0;

        virtual bool shouldCloseMainWindow() const = 0;

    protected:

        window_id_type m_MainWindowID = INVALID_WINDOW_ID;
        jmap_auto_id<window_id_type, WindowDescriptionBase*> m_Windows;


        virtual bool initInternal() = 0;
        virtual void terminateInternal();
        void terminateWindowDescriptions();

        template<typename T = WindowDescriptionBase, TEMPLATE_ENABLE(std::is_base_of_v<WindowDescriptionBase, T>)>
        T* getWindowDescription(const window_id_type windowID)
        {
            if (windowID != INVALID_WINDOW_ID)
            {
                WindowDescriptionBase* const* description = m_Windows.find(windowID);
                if (description != nullptr)
                {
                    return dynamic_cast<T*>(*description);
                }
            }
            return nullptr;
        }
        template<typename T = WindowDescriptionBase, TEMPLATE_ENABLE(std::is_base_of_v<WindowDescriptionBase, T>)>
        const T* getWindowDescription(const window_id_type windowID) const
        {
            if (windowID != INVALID_WINDOW_ID)
            {
                WindowDescriptionBase* const* description = m_Windows.find(windowID);
                if (description != nullptr)
                {
                    return dynamic_cast<const T*>(*description);
                }
            }
            return nullptr;
        }

        virtual WindowDescriptionBase* createWindow(const glm::uvec2& size, const jstring& title) = 0;

        virtual void setActiveWindowInCurrentThread(window_id_type windowID) = 0;

        virtual bool updateWindowSize(window_id_type windowID, const glm::uvec2& size) = 0;
        virtual bool updateWindowTitle(window_id_type windowID, const jstring& title) = 0;

    private:

        bool m_Initialized = false;
        bool m_Terminated = false;
    };
}
