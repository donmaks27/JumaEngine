// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "window/window_id.h"
#include "window/WindowDescriptionBase.h"
#include "utils/jmap_auto_id.h"
#include "engine/EngineContextObject.h"
#include "IRenderInterface.h"

#ifndef JUMAENGINE_SINGLE_WINDOW
#include "utils/jmutex_shared.h"
#endif

namespace JumaEngine
{
    class ShaderBase;
    class VertexBufferBase;
    class RenderTargetDirectBase;

    class RenderManagerBase : public EngineContextObject, public IRenderInterface
    {
        JUMAENGINE_CLASS(RenderManagerBase, EngineContextObject)

    public:
        RenderManagerBase() = default;
        virtual ~RenderManagerBase() override;

        bool init();
        bool isInit() const { return m_Initialized && !m_Terminated; }
        void terminate();

        window_id getMainWindowID() const { return m_MainWindowID; }
        bool isValidWindowID(const window_id windowID) const { return isInit() && m_Windows.contains(windowID); }

        window_id createWindow(const glm::uvec2& size, const jstring& title);

        bool shouldCloseMainWindow() const { return shouldCloseWindow(getMainWindowID()); }
        virtual bool shouldCloseWindow(window_id windowID) const = 0;
        void closeWindow(window_id windowID);
        void closeAllSecondaryWindows(bool destroyImmediately = false);

        bool getWindowSize(window_id windowID, glm::uvec2& outWindowSize) const;
        bool setWindowSize(window_id windowID, const glm::uvec2& size);

        bool getWindowTitle(window_id windowID, jstring& outWindowTitle) const;
        bool setWindowTitle(window_id windowID, const jstring& title);

        void setWindowRenderTarget(window_id windowID, RenderTargetBase* renderTarget);

        CameraComponent* getWindowActiveCamera(window_id windowID) const;
        void setWindowActiveCamera(window_id windowID, CameraComponent* camera);

        ShaderBase* createShader();

        VertexBufferBase* createVertextBuffer();
        void deleteVertexBuffer(VertexBufferBase* vertexBuffer);

        RenderTargetDirectBase* createRenderTargetDirect();
        
        void startRender();
        virtual void render(window_id windowID) override;

    protected:

        virtual bool initInternal() = 0;
        virtual void terminateInternal();
        void terminateWindowDescriptions();
        void terminateVertexBuffers();

        virtual WindowDescriptionBase* createWindowInternal(const glm::uvec2& size, const jstring& title) = 0;
        WindowDescriptionBase* getWindowDescriptionBase(window_id windowID);
        const WindowDescriptionBase* getWindowDescriptionBase(window_id windowID) const;
        template<typename T = WindowDescriptionBase, TEMPLATE_ENABLE(std::is_base_of_v<WindowDescriptionBase, T>)>
        T* getWindowDescription(const window_id windowID)
        {
            WindowDescriptionBase* description = getWindowDescriptionBase(windowID);
            return description != nullptr ? dynamic_cast<T*>(description) : nullptr;
        }
        template<typename T = WindowDescriptionBase, TEMPLATE_ENABLE(std::is_base_of_v<WindowDescriptionBase, T>)>
        const T* getWindowDescription(const window_id windowID) const
        {
            const WindowDescriptionBase* description = getWindowDescriptionBase(windowID);
            return description != nullptr ? dynamic_cast<const T*>(description) : nullptr;
        }

        virtual void markWindowShouldClose(window_id windowID) = 0;
#ifndef JUMAENGINE_SINGLE_WINDOW
        virtual void destroyWindowInternal(window_id windowID) = 0;
#endif

        virtual void setActiveWindowInCurrentThread(window_id windowID) = 0;

        virtual bool updateWindowSize(window_id windowID, const glm::uvec2& size) = 0;
        virtual bool updateWindowTitle(window_id windowID, const jstring& title) = 0;

        virtual ShaderBase* createShaderInternal() = 0;
        virtual VertexBufferBase* createVertextBufferInternal() = 0;
        virtual RenderTargetDirectBase* createRenderTargetDirectInternal() = 0;

    private:

        bool m_Initialized = false;
        bool m_Terminated = false;

        window_id m_MainWindowID = INVALID_WINDOW_ID;
        jmap_auto_id<window_id, WindowDescriptionBase*> m_Windows;

        jarray<VertexBufferBase*> m_VertexBuffers;

#ifndef JUMAENGINE_SINGLE_WINDOW
        mutable jmutex_shared m_WindowsListMutex;

        jarray<VertexBufferBase*> m_VertexBuffersForDelete;
        mutable jmutex_shared m_VertexBuffersMutex;


        void windowThreadFunction(window_id windowID);

        void destroyMarkedForCloseWindows();
        void destroyWindow(window_id windowID);
#endif
    };
}
