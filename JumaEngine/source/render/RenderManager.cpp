// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderManager.h"

namespace JumaEngine
{
    RenderManager::~RenderManager()
    {
        terminateWindowDescriptions();
    }

    bool RenderManager::init()
    {
        if (!m_Initialized)
        {
            m_Initialized = initInternal();
            return m_Initialized;
        }
        return false;
    }

    void RenderManager::terminate()
    {
        if (isInit())
        {
            terminateInternal();
            m_Terminated = true;
        }
    }
    void RenderManager::terminateInternal()
    {
        terminateWindowDescriptions();
    }
    void RenderManager::terminateWindowDescriptions()
    {
        for (auto& windowIDAndDescription : m_Windows)
        {
            delete windowIDAndDescription.second;
            windowIDAndDescription.second = nullptr;
        }
        m_Windows.clear();
        m_MainWindowID = INVALID_WINDOW_ID;
    }

    jarray<window_id_type> RenderManager::getWindowsList() const
    {
        jarray<window_id_type> result;
        if (isInit())
        {
            for (auto& windowIDAndDescription : m_Windows)
            {
                result.add(windowIDAndDescription.first);
            }
        }
        return result;
    }

    bool RenderManager::getWindowSize(const window_id_type windowID, glm::uvec2& outWindowSize) const
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
    bool RenderManager::setWindowSize(const window_id_type windowID, const glm::uvec2& size)
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

    bool RenderManager::getWindowTitle(window_id_type windowID, jstring& outWindowTitle) const
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
    bool RenderManager::setWindowTitle(window_id_type windowID, const jstring& title)
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
}
