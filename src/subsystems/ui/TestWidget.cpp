// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/subsystems/ui/TestWidget.h"

#include "JumaEngine/Engine.h"
#include "JumaEngine/subsystems/shaders/Material.h"
#include "JumaEngine/subsystems/ui/UISubsystem.h"
#include "JumaEngine/subsystems/ui/WidgetContext.h"

namespace JumaEngine
{
    void TestWidget::setMaterial(Material* material, const bool imitateCursor)
    {
        m_Material = material;
        m_ImitateCursor = imitateCursor;
    }

    void TestWidget::onUpdate(const float deltaTime)
    {
        Super::onUpdate(deltaTime);

        const WidgetContext* widgetContext = getWidgetContext();
        if ((widgetContext != nullptr) && m_ImitateCursor)
        {
            const JumaRE::RenderEngine* renderEngine = getEngine()->getRenderEngine();
            const JumaRE::WindowController* windowController = renderEngine->getWindowController();
            if (windowController->getCursorMode(windowController->getMainWindowID()) != JumaRE::WindowCursorMode::Locked)
            {
                const JumaRE::WindowData* windowData = windowController->findWindowData(windowController->getMainWindowID());
                const JumaRE::RenderTarget* renderTarget = widgetContext->getRenderTarget();
                const math::uvector2 renderTargetSize = renderTarget->getSize();
                const math::vector2 cursorPosition = windowData->cursorPosition;
                const math::vector2 screenCoordsModifier = math::vector2(1.0f, renderEngine->getRenderAPI() == JumaRE::RenderAPI::Vulkan ? 1.0f : -1.0f);
                const math::vector2 cursorLocation = (2.0f * (cursorPosition / renderTargetSize) - 1.0f) * screenCoordsModifier;

                setLocation(cursorLocation);
                setSize(math::vector2(24.0f, 24.0f) / renderTargetSize);
                if (renderEngine->getRenderAPI() == JumaRE::RenderAPI::Vulkan)
                {
                    setOffset({ 1.0f, 1.0f });
                }
                else
                {
                    setOffset({ 1.0f, -1.0f });
                }
                setVisibility(true);
            }
            else
            {
                setVisibility(false);
            }
        }
    }
    void TestWidget::onPostUpdate()
    {
        Super::onPostUpdate();

        const WidgetContext* widgetContext = getWidgetContext();
        if (widgetContext != nullptr)
        {
            updateMaterial();
            if (isVisible() && (m_Material != nullptr))
            {
                const Engine* engine = getEngine();
                const UISubsystem* uiSubsystem = engine->getSubsystem<UISubsystem>();
                if (uiSubsystem != nullptr)
                {
                    JumaRE::RenderTarget* renderTarget = widgetContext->getRenderTarget();
                    engine->getRenderEngine()->addPrimitiveToRenderList(renderTarget, uiSubsystem->getVertexBufferUI(), m_Material->getMaterial());
                }
            }
        }
    }
    void TestWidget::updateMaterial() const
    {
        static const jstringID locationParamName = JSTR("uLocation");
        static const jstringID offsetParamName = JSTR("uOffset");
        static const jstringID sizeParamName = JSTR("uSize");
        static const jstringID depthParamName = JSTR("uDepth");

        if (m_Material != nullptr)
        {
            m_Material->setParamValue<ShaderUniformType::Vec2>(locationParamName, m_Location);
            m_Material->setParamValue<ShaderUniformType::Vec2>(offsetParamName, m_Offset);
            m_Material->setParamValue<ShaderUniformType::Vec2>(sizeParamName, m_Size);
            m_Material->setParamValue<ShaderUniformType::Float>(depthParamName, m_Depth);
        }
    }
}
