// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/subsystems/ui/WidgetContainer.h"

#include "JumaEngine/Engine.h"
#include "JumaEngine/subsystems/ui/Widget.h"

namespace JumaEngine
{
    void WidgetContainer::setRenderTarget(JumaRE::RenderTarget* renderTarget)
    {
        if (!m_LogicInitialized)
        {
            m_RenderTarget = renderTarget;
        }
    }

    Widget* WidgetContainer::setRootWidget(const EngineSubclass<Widget>& widgetClass)
    {
        if (m_RootWidget != nullptr)
        {
            if (m_LogicInitialized)
            {
                m_RootWidget->onStopLogic();
            }
            delete m_RootWidget;
            m_RootWidget = nullptr;
        }

        m_RootWidget = getEngine()->createObject(widgetClass);
        if (m_RootWidget != nullptr)
        {
            if (m_LogicInitialized && !initRootWidget())
            {
                delete m_RootWidget;
                m_RootWidget = nullptr;
            }
            if (m_LogicStarted && (m_RootWidget != nullptr))
            {
                m_RootWidget->onStartLogic();
            }
        }

        return getRootWidget();
    }

    bool WidgetContainer::initLogicObject()
    {
        m_LogicInitialized = true;
        if (!Super::initLogicObject())
        {
            return false;
        }
        if (m_RenderTarget == nullptr)
        {
            return false;
        }
        return initRootWidget();
    }
    bool WidgetContainer::initRootWidget()
    {
        if (m_RootWidget != nullptr)
        {
            m_RootWidget->setWidgetContainer(this);
            if (!m_RootWidget->initLogicObject())
            {
                JUTILS_LOG(error, JSTR("Failed to init logic of root widget"));
                return false;
            }
        }
        return true;
    }

    void WidgetContainer::onStartLogic()
    {
        Super::onStartLogic();

        m_LogicStarted = true;
        if (m_RootWidget != nullptr)
        {
            m_RootWidget->onStartLogic();
        }
    }
    void WidgetContainer::onStopLogic()
    {
        Super::onStopLogic();

        m_LogicInitialized = false;
        m_LogicStarted = false;
        if (m_RootWidget != nullptr)
        {
            m_RootWidget->onStopLogic();
            delete m_RootWidget;
            m_RootWidget = nullptr;
        }
    }

    void WidgetContainer::update(const float deltaTime)
    {
        Super::update(deltaTime);

        if (m_RootWidget != nullptr)
        {
            m_RootWidget->update(deltaTime);
        }
    }
    void WidgetContainer::postUpdate()
    {
        Super::postUpdate();

        if (m_RootWidget != nullptr)
        {
            m_RootWidget->postUpdate();
        }
    }
}
