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
        terminateVertexBuffers();
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
    void RenderManagerBase::terminateVertexBuffers()
    {
        for (auto& vertexBuffer : m_VertexBuffers)
        {
            delete vertexBuffer;
        }
        m_VertexBuffers.clear();
        m_VertexBuffersForDelete.clear();
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

    void RenderManagerBase::closeWindow(const window_id windowID, const bool destroyImmediately)
    {
        if (isInit() && isMainThread())
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
        if (isInit() && isMainThread())
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

    ShaderBase* RenderManagerBase::createShader()
    {
        return isInit() ? createShaderInternal() : nullptr;
    }

    VertexBufferBase* RenderManagerBase::createVertextBuffer()
    {
        if (isInit())
        {
            VertexBufferBase* vertexBuffer = createVertextBufferInternal();

            m_VertexBuffersMutex.lock();
            m_VertexBuffers.add(vertexBuffer);
            m_VertexBuffersMutex.unlock();

            return vertexBuffer;
        }
        return nullptr;
    }
    void RenderManagerBase::deleteVertexBuffer(VertexBufferBase* vertexBuffer)
    {
        if (isInit() && isMainThread() && (vertexBuffer != nullptr))
        {
            vertexBuffer->clearWindowData(getMainWindowID());

            m_VertexBuffersMutex.lock();
            if (vertexBuffer->hasAnyWindowData())
            {
                m_VertexBuffersForDelete.add(vertexBuffer);
            }
            else
            {
                m_VertexBuffers.remove(vertexBuffer);
                delete vertexBuffer;
            }
            m_VertexBuffersMutex.unlock();
        }
    }

    RenderTargetDirectBase* RenderManagerBase::createRenderTargetDirect()
    {
        return createRenderTargetDirectInternal();
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

        m_VertexBuffersMutex.lock();
        uint32 index = 0;
        while (index < m_VertexBuffersForDelete.size())
        {
            VertexBufferBase* vertexBuffer = m_VertexBuffersForDelete[index];
            if (!vertexBuffer->hasAnyWindowData())
            {
                m_VertexBuffersForDelete.removeAt(index);
                m_VertexBuffers.remove(vertexBuffer);
                delete vertexBuffer;
            }
            else
            {
                ++index;
            }
        }
        m_VertexBuffersMutex.unlock();
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
                for (auto& vertexBuffer : m_VertexBuffersForDelete)
                {
                    vertexBuffer->clearWindowData(windowID);
                }

                windowDescription->windowShouldStartRender.store(false);
                render(windowID);
                windowDescription->windowRenderFinish.store(true);
            }
        }

        m_VertexBuffersMutex.lock_shared();
        for (auto& vertexBuffer : m_VertexBuffers)
        {
            vertexBuffer->clearWindowData(windowID);
        }
        m_VertexBuffersMutex.unlock_shared();

        setActiveWindowInCurrentThread(INVALID_WINDOW_ID);
        windowDescription->windowActive.store(false);
    }

    void RenderManagerBase::render(const window_id windowID)
    {
        WindowDescriptionBase* description = getWindowDescription(windowID);
        if (description != nullptr)
        {
            description->windowRenderTarget->render(windowID);
        }
    }
}
