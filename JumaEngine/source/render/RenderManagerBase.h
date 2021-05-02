// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "window/window_id.h"
#include "window/WindowDescriptionBase.h"
#include "utils/jmap_auto_id.h"
#include "EngineContextObject.h"
#include "IRenderInterface.h"

namespace JumaEngine
{
    class ShaderBase;
    class VertexBufferBase;

    class RenderManagerBase : public EngineContextObject, public IRenderInterface
    {
    public:
        RenderManagerBase() = default;
        virtual ~RenderManagerBase() override;

        bool init();
        bool isInit() const { return m_Initialized && !m_Terminated; }
        void terminate();

        window_id getMainWindowID() const { return m_MainWindowID; }
        bool isValidWindowID(const window_id windowID) const { return isInit() && m_Windows.contains(windowID); }

        bool getWindowSize(window_id windowID, glm::uvec2& outWindowSize) const;
        bool setWindowSize(window_id windowID, const glm::uvec2& size);

        bool getWindowTitle(window_id windowID, jstring& outWindowTitle) const;
        bool setWindowTitle(window_id windowID, const jstring& title);
        
        virtual ShaderBase* createShader() = 0;
        virtual VertexBufferBase* createVertextBuffer() = 0;

        virtual void startRender();
        virtual void render(window_id windowID) override;
        virtual void finishRender() = 0;

        virtual bool shouldCloseMainWindow() const = 0;

        void setWindowRenderTarget(window_id windowID, RenderTargetBase* renderTarget);

        CameraComponent* getWindowActiveCamera(window_id windowID) const;
        void setWindowActiveCamera(window_id windowID, CameraComponent* camera);

    protected:

        window_id m_MainWindowID = INVALID_WINDOW_ID;
        jmap_auto_id<window_id, WindowDescriptionBase*> m_Windows;


        virtual bool initInternal() = 0;
        virtual void terminateInternal();
        void terminateWindowDescriptions();

        template<typename T = WindowDescriptionBase, TEMPLATE_ENABLE(std::is_base_of_v<WindowDescriptionBase, T>)>
        T* getWindowDescription(const window_id windowID)
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
        const T* getWindowDescription(const window_id windowID) const
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

        virtual void setActiveWindowInCurrentThread(window_id windowID) = 0;

        virtual bool updateWindowSize(window_id windowID, const glm::uvec2& size) = 0;
        virtual bool updateWindowTitle(window_id windowID, const jstring& title) = 0;

    private:

        bool m_Initialized = false;
        bool m_Terminated = false;
    };
}
