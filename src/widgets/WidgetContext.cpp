// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/widgets/WidgetContext.h"

#include "JumaEngine/widgets/Widget.h"

namespace JumaEngine
{
    void WidgetContext::onActivated()
    {
        Super::onActivated();

        if (m_RootWidget != nullptr)
        {
            ActivateEngineObject(m_RootWidget.get());
        }
    }
    void WidgetContext::onUpdate(const float deltaTime)
    {
        Super::onUpdate(deltaTime);

        if (m_RootWidget != nullptr)
        {
            UpdateEngineObject(m_RootWidget.get(), deltaTime);
        }
    }
    void WidgetContext::onPreRender()
    {
        Super::onPreRender();

        if (m_RootWidget != nullptr)
        {
            m_RootWidget->setWidgetBounds({ { 0.0f, 0.0f }, { 1.0f, 1.0f } }, WidgetAlignmentH::Fill, WidgetAlignmentV::Fill);
            m_RootWidget->recalculateWidetSize();

            PreRenderEngineObject(m_RootWidget.get());
        }
    }
    void WidgetContext::onDeactivate()
    {
        if (m_RootWidget != nullptr)
        {
            DeactivateEngineObject(m_RootWidget.get());
        }

        Super::onDeactivate();
    }
    void WidgetContext::onClear()
    {
        setRootWidget(nullptr);
        m_RenderContext = RenderContext();
        m_ParentWidgetsCreator = nullptr;

        Super::onClear();
    }

    void WidgetContext::setRootWidget(EngineObjectPtr<Widget> widget)
    {
        if (m_RootWidget == widget)
        {
            return;
        }
        if ((widget != nullptr) && (widget->getWidgetsCreator() != getWidgetsCreator()))
        {
            JUTILS_LOG(warning, JSTR("Widget is not owned by WidgetsCreator of this WidgetContext"));
            return;
        }

        if (m_RootWidget != nullptr)
        {
            m_RootWidget->onWidgetContextChanged.unbind(this, &WidgetContext::onRootWidgetContextChanging);
            m_RootWidget->onDestroying.unbind(this, &WidgetContext::onRootWidgetDestroying);

            DeactivateEngineObject(m_RootWidget.get());
            m_RootWidget->setWidgetContext(nullptr);
        }
        m_RootWidget = std::move(widget);
        if (m_RootWidget != nullptr)
        {
            m_RootWidget->setParentWidget(nullptr);
            m_RootWidget->setWidgetContext(this);

            if (isActive())
            {
                m_RootWidget->onWidgetContextChanged.bind(this, &WidgetContext::onRootWidgetContextChanging);
                m_RootWidget->onDestroying.bind(this, &WidgetContext::onRootWidgetDestroying);
                ActivateEngineObject(m_RootWidget.get());
            }
        }
    }

    void WidgetContext::destroy()
    {
        ClearEngineObject(this);
    }
}
