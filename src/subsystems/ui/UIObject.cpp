// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#include "JumaEngine/subsystems/ui/UIObject.h"

#include "JumaEngine/Engine.h"
#include "JumaEngine/subsystems/ui/UIElement.h"
#include "JumaEngine/subsystems/ui/UISubsystem.h"

namespace JumaEngine
{
    UIObject::~UIObject()
    {
        for (const auto& element : m_UIElements)
        {
            delete element;
        }
        m_UIElements.clear();
        m_RenderTarget = nullptr;
    }

    UIElement* UIObject::addElement()
    {
        return m_UIElements.add(getEngine()->createObject<UIElement>());
    }

    void UIObject::update()
    {
        if (m_RenderTarget != nullptr)
        {
            const Engine* engine = getEngine();
            JumaRE::RenderEngine* renderEngine = engine->getRenderEngine();
            JumaRE::VertexBuffer* vertexBuffer = engine->getSubsystem<UISubsystem>()->getVertexBufferUI();
            for (const auto& element : m_UIElements)
            {
                const Material* material = element->getMaterial();
                if (material != nullptr)
                {
                    element->updateMaterial();
                    renderEngine->addPrimitiveToRenderList(m_RenderTarget, vertexBuffer, material->getMaterial());
                }
            }
        }
    }
}
