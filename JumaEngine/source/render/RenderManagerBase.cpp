// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderManagerBase.h"
#include "renderTarget/RenderTargetBase.h"

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
        render(getMainWindowID());
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
}
