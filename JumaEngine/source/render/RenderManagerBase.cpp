// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderManagerBase.h"
#include "renderTarget/RenderTargetBase.h"
#include "vertexBuffer/VertexBufferBase.h"

namespace JumaEngine
{
    RenderManagerBase::~RenderManagerBase()
    {
        terminateWindowDescriptions();
        terminateVertexBuffers();
    }

    void RenderManagerBase::onRegister()
    {
        Super::onRegister();

        m_Initialized = initInternal();
    }

    void RenderManagerBase::terminate()
    {
        if (isInit())
        {
#ifndef JUMAENGINE_SINGLE_WINDOW
            closeAllSecondaryWindows(true);
#endif
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
#ifndef JUMAENGINE_SINGLE_WINDOW
        m_VertexBuffersForDelete.clear();
#endif
    }

    window_id RenderManagerBase::createWindow(const glm::uvec2& size, const jstring& title)
    {
#ifndef JUMAENGINE_SINGLE_WINDOW
        if (isMainThread())
        {
            WindowDescriptionBaseOld* description = createWindowInternal(size, title);
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
#else
        if (m_MainWindowID == INVALID_WINDOW_ID)
        {
            WindowDescriptionBaseOld* description = createWindowInternal(size, title);
            if (description != nullptr)
            {
                m_MainWindowID = m_Windows.add(description);
                setActiveWindowInCurrentThread(m_MainWindowID);
                return m_MainWindowID;
            }
        }
#endif
        return INVALID_WINDOW_ID;
    }

    WindowDescriptionBaseOld* RenderManagerBase::getWindowDescriptionBase(const window_id windowID)
    {
#ifndef JUMAENGINE_SINGLE_WINDOW
        if (windowID != INVALID_WINDOW_ID)
        {
            const bool shouldUseMutex = !isMainThread();

            if (shouldUseMutex)
            {
                m_WindowsListMutex.lock_shared();
            }
            WindowDescriptionBaseOld* const* description = m_Windows.find(windowID);
            WindowDescriptionBaseOld* result = description != nullptr ? *description : nullptr;
            if (shouldUseMutex)
            {
                m_WindowsListMutex.unlock_shared();
            }

            return result;
        }
#else
        if ((windowID != INVALID_WINDOW_ID) && (windowID == getMainWindowID()))
        {
            return *m_Windows.find(windowID);
        }
#endif
        return nullptr;
    }
    const WindowDescriptionBaseOld* RenderManagerBase::getWindowDescriptionBase(const window_id windowID) const
    {
#ifndef JUMAENGINE_SINGLE_WINDOW
        if (windowID != INVALID_WINDOW_ID)
        {
            const bool shouldUseMutex = !isMainThread();

            if (shouldUseMutex)
            {
                m_WindowsListMutex.lock_shared();
            }
            WindowDescriptionBaseOld* const* description = m_Windows.find(windowID);
            const WindowDescriptionBaseOld* result = description != nullptr ? *description : nullptr;
            if (shouldUseMutex)
            {
                m_WindowsListMutex.unlock_shared();
            }

            return result;
        }
#else
        if ((windowID != INVALID_WINDOW_ID) && (windowID == getMainWindowID()))
        {
            return *m_Windows.find(windowID);
        }
#endif
        return nullptr;
    }

    void RenderManagerBase::closeWindow(const window_id windowID)
    {
        if (isInit())
        {
            markWindowShouldClose(windowID);
#ifndef JUMAENGINE_SINGLE_WINDOW
            if (isMainThread() && (windowID != getMainWindowID()) && shouldCloseMainWindow())
            {
                destroyWindow(windowID);
            }
#endif
        }
    }
    void RenderManagerBase::closeAllSecondaryWindows(const bool destroyImmediately)
    {
#ifndef JUMAENGINE_SINGLE_WINDOW
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
#endif
    }
#ifndef JUMAENGINE_SINGLE_WINDOW
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
                    WindowDescriptionBaseOld*& description = windowIDAndDescription.second;
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
        WindowDescriptionBaseOld* description = getWindowDescriptionBase(windowID);
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
#endif

    bool RenderManagerBase::getWindowSize(const window_id windowID, glm::uvec2& outWindowSize) const
    {
        if (isInit())
        {
            const WindowDescriptionBaseOld* windowDescription = getWindowDescription(windowID);
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
            WindowDescriptionBaseOld* windowDescription = getWindowDescription(windowID);
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
            const WindowDescriptionBaseOld* windowDescription = getWindowDescription(windowID);
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
            WindowDescriptionBaseOld* windowDescription = getWindowDescription(windowID);
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
        WindowDescriptionBaseOld* description = getWindowDescription(windowID);
        if (description != nullptr)
        {
            description->windowRenderTarget = renderTarget;
        }
    }

    CameraComponent* RenderManagerBase::getWindowActiveCamera(const window_id windowID) const
    {
        const WindowDescriptionBaseOld* description = getWindowDescription(windowID);
        return description != nullptr ? description->activeCamera : nullptr;
    }
    void RenderManagerBase::setWindowActiveCamera(const window_id windowID, CameraComponent* camera)
    {
        WindowDescriptionBaseOld* description = getWindowDescription(windowID);
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

#ifndef JUMAENGINE_SINGLE_WINDOW
            m_VertexBuffersMutex.lock();
            m_VertexBuffers.add(vertexBuffer);
            m_VertexBuffersMutex.unlock();
#else
            m_VertexBuffers.add(vertexBuffer);
#endif

            return vertexBuffer;
        }
        return nullptr;
    }
    void RenderManagerBase::deleteVertexBuffer(VertexBufferBase* vertexBuffer)
    {
        if (isInit() && (vertexBuffer != nullptr))
        {
#ifndef JUMAENGINE_SINGLE_WINDOW
            if (isMainThread())
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
#else
            vertexBuffer->clearWindowData(getMainWindowID());
            m_VertexBuffers.remove(vertexBuffer);
            delete vertexBuffer;
#endif
        }
    }

    TextureBase* RenderManagerBase::createTexture()
    {
        return createTextureInternal();
    }

    RenderTargetDirectBase* RenderManagerBase::createRenderTargetDirect()
    {
        return createRenderTargetDirectInternal();
    }

    void RenderManagerBase::startRender()
    {
        const window_id mainWindowID = getMainWindowID();

#ifndef JUMAENGINE_SINGLE_WINDOW
        destroyMarkedForCloseWindows();
        for (auto& windowIDAndDescription : m_Windows)
        {
            if (windowIDAndDescription.first != mainWindowID)
            {
                WindowDescriptionBaseOld*& description = windowIDAndDescription.second;
                if (description->windowActive.load())
                {
                    description->windowShouldStartRender.store(true);
                }
            }
        }
#endif

        render(mainWindowID, { false });

#ifndef JUMAENGINE_SINGLE_WINDOW
        for (auto& windowIDAndDescription : m_Windows)
        {
            if (windowIDAndDescription.first != mainWindowID)
            {
                WindowDescriptionBaseOld*& description = windowIDAndDescription.second;
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
#endif
    }
#ifndef JUMAENGINE_SINGLE_WINDOW
    void RenderManagerBase::windowThreadFunction(const window_id windowID)
    {
        WindowDescriptionBaseOld* windowDescription = getWindowDescriptionBase(windowID);
        WindowDescriptionBaseOld* mainWindowDescription = getWindowDescriptionBase(getMainWindowID());

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
#endif

    void RenderManagerBase::render(const window_id windowID, const RenderParams& renderParams)
    {
        WindowDescriptionBaseOld* description = getWindowDescription(windowID);
        if (description != nullptr)
        {
            description->windowRenderTarget->render(windowID, renderParams);
        }
    }
}
