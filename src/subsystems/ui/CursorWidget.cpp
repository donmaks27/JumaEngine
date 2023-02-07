// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/subsystems/ui/CursorWidget.h"

#include "JumaEngine/Engine.h"
#include "JumaEngine/subsystems/shaders/Material.h"
#include "JumaEngine/subsystems/shaders/ShadersSubsystem.h"
#include "JumaEngine/subsystems/ui/UISubsystem.h"
#include "JumaEngine/subsystems/ui/WidgetContext.h"

namespace JumaEngine
{
    void CursorWidget::onInitialized()
    {
	    Super::onInitialized();

        ShadersSubsystem* shadersSubsystem = getEngine()->getSubsystem<ShadersSubsystem>();
        Shader* shader = shadersSubsystem->getEngineShader(JSTR("widgetSolidColor"));
        if (shader != nullptr)
        {
	        m_Material = shadersSubsystem->createMaterial(shader);
            m_Material->setParamValue<ShaderUniformType::Vec4>(JSTR("uColor"), { 1.0f, 0.0f, 1.0f, 1.0f });
        }
    }

    void CursorWidget::onUpdate(const float deltaTime)
    {
        Super::onUpdate(deltaTime);
        
        const JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
        const JumaRE::WindowController* windowController = renderEngine->getWindowController();
        if (windowController->getCursorMode(windowController->getMainWindowID()) != JumaRE::WindowCursorMode::Locked)
        {
            m_CursorVisible = true;

            const JumaRE::WindowData* windowData = windowController->findWindowData(windowController->getMainWindowID());
            if (renderEngine->getRenderAPI() != JumaRE::RenderAPI::OpenGL)
            {
	            m_CursorLocation = math::vector2(windowData->cursorPosition) / windowData->size;
            }
            else
            {
	            m_CursorLocation.x = static_cast<float>(windowData->cursorPosition.x) / static_cast<float>(windowData->size.x);
	            m_CursorLocation.y = static_cast<float>(windowData->cursorPosition.y - m_CursorSizePixels.y) / static_cast<float>(windowData->size.y);
            }
            //m_CursorLocation = math::vector2(windowData->cursorPosition) / windowData->size;
        }
        else
        {
            m_CursorVisible = false;
        }
    }
    void CursorWidget::onPreRender()
    {
        Super::onPreRender();

        const WidgetContext* widgetContext = getWidgetContext();
        if (widgetContext != nullptr)
        {
            updateMaterial();
            if (m_CursorVisible && (m_Material != nullptr))
            {
                const UISubsystem* uiSubsystem = getEngine()->getSubsystem<UISubsystem>();
                if (uiSubsystem != nullptr)
                {
                    widgetContext->getRenderTarget()->addPrimitiveToRenderList({ uiSubsystem->getVertexBufferUI(), m_Material->getMaterial() });
                }
            }
        }
    }

    void CursorWidget::onDestroying()
    {
        if (m_Material != nullptr)
        {
	        getEngine()->getSubsystem<ShadersSubsystem>()->destroyMaterial(m_Material);
            m_Material = nullptr;
        }

	    Super::onDestroying();
    }

    void CursorWidget::recalculateWidetSize()
    {
        const WidgetContext* widgetContext = getWidgetContext();
        const JumaRE::RenderTarget* renderTarget = widgetContext != nullptr ? widgetContext->getRenderTarget() : nullptr;
        if (renderTarget == nullptr)
        {
	        m_WidgetRenderSize = { 0.0f, 0.0f };
        }
        else
        {
	        const math::box2 bounds = getWidgetBounds();
            m_WidgetRenderSize = math::vector2(m_CursorSizePixels) / renderTarget->getSize() * (bounds.v1 - bounds.v0);
        }
    }
    math::vector2 CursorWidget::getWidgetRenderLocation() const
    {
	    const math::box2 bounds = getWidgetBounds();
        return bounds.v0 + m_CursorLocation * (bounds.v1 - bounds.v0);
    }

    void CursorWidget::updateMaterial() const
    {
        static const jstringID locationParamName = JSTR("uLocation");
        static const jstringID sizeParamName = JSTR("uSize");
        static const jstringID depthParamName = JSTR("uDepth");

        if (m_Material != nullptr)
        {
            m_Material->setParamValue<ShaderUniformType::Vec2>(locationParamName, getWidgetRenderLocation());
            m_Material->setParamValue<ShaderUniformType::Vec2>(sizeParamName, getWidgetRenderSize());
            m_Material->setParamValue<ShaderUniformType::Float>(depthParamName, m_Depth);
        }
    }
}
