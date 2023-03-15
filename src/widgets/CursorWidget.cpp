// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/widgets/CursorWidget.h"

#include "JumaEngine/engine/Engine.h"
#include "JumaEngine/assets/AssetsEngineSubsystem.h"
#include "JumaEngine/widgets/WidgetContext.h"

namespace JumaEngine
{
    void CursorWidget::onInitialized()
    {
	    Super::onInitialized();

        AssetsEngineSubsystem* assetsSubsystem = getEngine()->getSubsystem<AssetsEngineSubsystem>();
        m_Material = assetsSubsystem->createMaterial(assetsSubsystem->getMaterialAsset(JSTR("e:materials/M_widgetSolidColor")));
        if (m_Material != nullptr)
        {
	        m_Material->setParamValue<MaterialParamType::Vec4>(JSTR("color"), { 1.0f, 0.0f, 1.0f, 1.0f });
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
	            m_CursorLocation.y = static_cast<float>(static_cast<int64>(windowData->cursorPosition.y) - static_cast<int64>(m_CursorSizePixels.y)) / static_cast<float>(windowData->size.y);
            }
        }
        else
        {
            m_CursorVisible = false;
        }
    }
    void CursorWidget::onPreRender()
    {
        Super::onPreRender();

        const WidgetContext* widgetContext = getWidgetContextPtr();
        if (widgetContext != nullptr)
        {
            updateMaterial();
            if (m_CursorVisible && (m_Material != nullptr))
            {
                AssetsEngineSubsystem* assetsSubsystem = getEngine()->getSubsystem<AssetsEngineSubsystem>();
                if (assetsSubsystem != nullptr)
                {
                    widgetContext->getRenderContext().addPrimitiveToRenderList({
	                    assetsSubsystem->getVertexBuffer_Plane2D(), m_Material->getMaterial()
                    });
                }
            }
        }
    }

    void CursorWidget::onClear()
    {
        m_Material = nullptr;

	    Super::onClear();
    }

    void CursorWidget::recalculateWidetSize()
    {
        const WidgetContext* widgetContext = getWidgetContextPtr();
        const EngineObjectPtr<RenderTarget>& renderTarget = widgetContext != nullptr ? widgetContext->getRenderTarget() : nullptr;
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
        math::vector2 result = m_CursorLocation * (bounds.v1 - bounds.v0);
        if (getEngine()->getRenderEngine()->getRenderAPI() != JumaRE::RenderAPI::OpenGL)
        {
	        return bounds.v0 + result;
        }

        result.x += bounds.v0.x;
        result.y = bounds.v1.y - result.y;
        return result;
    }

    void CursorWidget::updateMaterial() const
    {
        static const jstringID locationParamName = JSTR("uLocation");
        static const jstringID sizeParamName = JSTR("uSize");
        static const jstringID depthParamName = JSTR("uDepth");

        if (m_Material != nullptr)
        {
            m_Material->getMaterial()->setParamValue<JumaRE::ShaderUniformType::Vec2>(locationParamName, getWidgetRenderLocation());
            m_Material->getMaterial()->setParamValue<JumaRE::ShaderUniformType::Vec2>(sizeParamName, getWidgetRenderSize());
            m_Material->getMaterial()->setParamValue<JumaRE::ShaderUniformType::Float>(depthParamName, m_Depth);
        }
    }
}
