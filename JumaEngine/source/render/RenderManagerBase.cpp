// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderManagerBase.h"
#include "renderTarget/RenderTargetBase.h"
#include "vertexBuffer/VertexBufferBase.h"

namespace JumaEngine
{
    RenderManagerBase::~RenderManagerBase()
    {
        terminateWindowDescriptions();
    }

    bool RenderManagerBase::init()
    {
        if (!m_Initialized)
        {
            m_Initialized = initInternal();
            return m_Initialized;
        }
        return false;
    }

    void RenderManagerBase::terminate()
    {
        if (isInit())
        {
            closeAllSecondaryWindows(true);
            terminateInternal();
            m_Terminated = true;
        }
    }
    void RenderManagerBase::terminateInternal()
    {
        terminateWindowDescriptions();
    }
    void RenderManagerBase::terminateWindowDescriptions()
    {
        for (auto& windowIDAndDescription : m_Windows)
        {
            delete windowIDAndDescription.second;
            windowIDAndDescription.second = nullptr;
        }
        m_Windows.clear();
        m_MainWindowID = INVALID_WINDOW_ID;
    }

    WindowDescriptionBase* RenderManagerBase::getWindowDescriptionBase(const window_id windowID)
    {
        if (windowID != INVALID_WINDOW_ID)
        {
            const bool shouldUseMutex = !isMainThread();

            if (shouldUseMutex)
            {
                m_WindowsListMutex.lock_shared();
            }
            WindowDescriptionBase* const* description = m_Windows.find(windowID);
            WindowDescriptionBase* result = description != nullptr ? *description : nullptr;
            if (shouldUseMutex)
            {
                m_WindowsListMutex.unlock_shared();
            }

            return result;
        }
        return nullptr;
    }
    const WindowDescriptionBase* RenderManagerBase::getWindowDescriptionBase(const window_id windowID) const
    {
        if (windowID != INVALID_WINDOW_ID)
        {
            const bool shouldUseMutex = !isMainThread();

            if (shouldUseMutex)
            {
                m_WindowsListMutex.lock_shared();
            }
            WindowDescriptionBase* const* description = m_Windows.find(windowID);
            const WindowDescriptionBase* result = description != nullptr ? *description : nullptr;
            if (shouldUseMutex)
            {
                m_WindowsListMutex.unlock_shared();
            }

            return result;
        }
        return nullptr;
    }

    window_id RenderManagerBase::createWindow(const glm::uvec2& size, const jstring& title)
    {
        if (isMainThread())
        {
            WindowDescriptionBase* description = createWindowInternal(size, title);
            if (description != nullptr)
            {
                m_WindowsListMutex.lock();

                const window_id windowID = m_Windows.add(description);
                if (m_MainWindowID == INVALID_WINDOW_ID)
                {
                    m_MainWindowID = windowID;
                    setActiveWindowInCurrentThread(windowID);
                }
                else
                {
                    description->windowActive.store(true);
                    description->windowThread = std::thread(&RenderManagerBase::windowThreadFunction, this, windowID);
                }

                m_WindowsListMutex.unlock();

                return windowID;
            }
        }
        return INVALID_WINDOW_ID;
    }
    void RenderManagerBase::closeWindow(const window_id windowID, const bool destroyImmediately)
    {
        if (isMainThread())
        {
            markWindowShouldClose(windowID);
            if ((windowID != getMainWindowID()) && (destroyImmediately || shouldCloseMainWindow()))
            {
                destroyWindow(windowID);
            }
        }
    }
    void RenderManagerBase::closeAllSecondaryWindows(const bool destroyImmediately)
    {
        if (isMainThread())
        {
            const window_id mainWindowID = getMainWindowID();
            for (const auto& windowID : m_Windows.getIDs())
            {
                if (windowID != mainWindowID)
                {
                    closeWindow(windowID);
                }
            }
        }
    }

    bool RenderManagerBase::getWindowSize(const window_id windowID, glm::uvec2& outWindowSize) const
    {
        if (isInit())
        {
            const WindowDescriptionBase* windowDescription = getWindowDescription(windowID);
            if (windowDescription != nullptr)
            {
                outWindowSize = windowDescription->windowSize;
                return true;
            }
        }
        return false;
    }
    bool RenderManagerBase::setWindowSize(const window_id windowID, const glm::uvec2& size)
    {
        if (isInit())
        {
            WindowDescriptionBase* windowDescription = getWindowDescription(windowID);
            if ((windowDescription != nullptr) && (windowDescription->windowSize != size))
            {
                if (updateWindowSize(windowID, size))
                {
                    windowDescription->windowSize = size;
                    return true;
                }
            }
        }
        return false;
    }

    bool RenderManagerBase::getWindowTitle(window_id windowID, jstring& outWindowTitle) const
    {
        if (isInit())
        {
            const WindowDescriptionBase* windowDescription = getWindowDescription(windowID);
            if (windowDescription != nullptr)
            {
                outWindowTitle = windowDescription->windowTitle;
                return true;
            }
        }
        return false;
    }
    bool RenderManagerBase::setWindowTitle(window_id windowID, const jstring& title)
    {
        if (isInit())
        {
            WindowDescriptionBase* windowDescription = getWindowDescription(windowID);
            if ((windowDescription != nullptr) && (windowDescription->windowTitle != title))
            {
                if (updateWindowTitle(windowID, title))
                {
                    windowDescription->windowTitle = title;
                    return true;
                }
            }
        }
        return false;
    }

    void RenderManagerBase::startRender()
    {
        const window_id mainWindowID = getMainWindowID();
        WindowDescriptionBase* mainWindowDescription = getWindowDescriptionBase(mainWindowID);
        if (mainWindowDescription == nullptr)
        {
            return;
        }

        destroyMarkedForCloseWindows();

        for (auto& windowIDAndDescription : m_Windows)
        {
            if (windowIDAndDescription.first != mainWindowID)
            {
                WindowDescriptionBase*& description = windowIDAndDescription.second;
                if (description->windowActive.load())
                {
                    description->windowShouldStartRender.store(true);
                }
            }
        }

        render(mainWindowID);
        
        for (auto& windowIDAndDescription : m_Windows)
        {
            if (windowIDAndDescription.first != mainWindowID)
            {
                WindowDescriptionBase*& description = windowIDAndDescription.second;
                while (!description->windowRenderFinish.load() && description->windowActive.load()) {}
                description->windowRenderFinish.store(false);
            }
        }
    }
    void RenderManagerBase::windowThreadFunction(const window_id windowID)
    {
        WindowDescriptionBase* windowDescription = getWindowDescriptionBase(windowID);
        WindowDescriptionBase* mainWindowDescription = getWindowDescriptionBase(getMainWindowID());

        setActiveWindowInCurrentThread(windowID);

        while (!shouldCloseWindow(windowID))
        {
            if (windowDescription->windowShouldStartRender.load())
            {
                windowDescription->windowShouldStartRender.store(false);
                render(windowID);
                windowDescription->windowRenderFinish.store(true);
            }
        }

        setActiveWindowInCurrentThread(INVALID_WINDOW_ID);
        windowDescription->windowActive.store(false);
    }

    void RenderManagerBase::destroyMarkedForCloseWindows()
    {
        if (isMainThread())
        {
            const window_id mainWindowID = getMainWindowID();

            jarray<window_id> markedWindowIDs;
            for (auto& windowIDAndDescription : m_Windows)
            {
                const window_id windowID = windowIDAndDescription.first;
                if (windowID != mainWindowID)
                {
                    WindowDescriptionBase*& description = windowIDAndDescription.second;
                    if (!description->windowActive.load())
                    {
                        markedWindowIDs.add(windowID);
                    }
                }
            }

            for (const auto& windowID : markedWindowIDs)
            {
                destroyWindow(windowID);
            }
        }
    }
    void RenderManagerBase::destroyWindow(const window_id windowID)
    {
        WindowDescriptionBase* description = getWindowDescriptionBase(windowID);
        if (description != nullptr)
        {
            destroyWindowInternal(windowID);
            description->windowThread.join();

            m_WindowsListMutex.lock();

            delete description;
            m_Windows.remove(windowID);

            m_WindowsListMutex.unlock();
        }
    }

    void RenderManagerBase::render(const window_id windowID)
    {
        WindowDescriptionBase* description = getWindowDescription(windowID);
        if (description != nullptr)
        {
            description->windowRenderTarget->render(windowID);
        }
    }

    void RenderManagerBase::setWindowRenderTarget(const window_id windowID, RenderTargetBase* renderTarget)
    {
        WindowDescriptionBase* description = getWindowDescription(windowID);
        if (description != nullptr)
        {
            description->windowRenderTarget = renderTarget;
        }
    }

    CameraComponent* RenderManagerBase::getWindowActiveCamera(const window_id windowID) const
    {
        const WindowDescriptionBase* description = getWindowDescription(windowID);
        return description != nullptr ? description->activeCamera : nullptr;
    }
    void RenderManagerBase::setWindowActiveCamera(const window_id windowID, CameraComponent* camera)
    {
        WindowDescriptionBase* description = getWindowDescription(windowID);
        if (description != nullptr)
        {
            description->activeCamera = camera;
        }
    }

    ShaderBase* RenderManagerBase::getWindowActiveShader(const window_id windowID) const
    {
        const WindowDescriptionBase* description = getWindowDescription(windowID);
        return description != nullptr ? description->activeShader : nullptr;
    }
    void RenderManagerBase::setWindowActiveShader(const window_id windowID, ShaderBase* shader)
    {
        WindowDescriptionBase* description = getWindowDescription(windowID);
        if (description != nullptr)
        {
            description->activeShader = shader;
        }
    }

    void RenderManagerBase::deleteVertexBuffer(VertexBufferBase* vertexBuffer)
    {
        if ((vertexBuffer != nullptr) && vertexBuffer->isInit())
        {
            vertexBuffer->clearWindowData(getMainWindowID());
            delete vertexBuffer;
        }
    }
}
