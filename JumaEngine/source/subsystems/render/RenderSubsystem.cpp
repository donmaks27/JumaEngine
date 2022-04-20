// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderSubsystem.h"

#include "RenderPipeline.h"
#include "RenderSubsystem_Implementation.h"
#include "Texture.h"
#include "engine/Engine.h"
#include "subsystems/window/WindowSubsystem.h"

namespace JumaEngine
{
    void RenderSubsystem_RenderAPIObject::clearData()
    {
        RenderPipeline* renderPipeline = m_Parent->getRenderPipeline();
        if (renderPipeline != nullptr)
        {
            renderPipeline->clearRenderAPIObject();
        }
        Texture* defaultTexture = m_Parent->getDefaultTexture();
        if (defaultTexture != nullptr)
        {
            defaultTexture->clearRenderAPIObject();
        }
    }

    RenderSubsystem_RenderAPIObject* RenderSubsystem::createRenderAPIObjectInternal()
    {
        return createRenderSubsystemRenderAPIObject(getRenderAPI());
    }
    
    bool RenderSubsystem::initSubsystemInternal()
    {
        if (!Super::initSubsystemInternal())
        {
            return false;
        }

        m_DefaultTexture = getOwnerEngine()->createObject<Texture>();
        m_DefaultTexture->init(TextureFormat::RGBA, { 1, 1 }, new uint8[4]{ 255, 0, 255, 0 });

        m_RenderPipeline = getOwnerEngine()->createObject<RenderPipeline>();
        m_RenderPipeline->init();
        return true;
    }
    void RenderSubsystem::clearSubsystemInternal()
    {
        delete m_RenderPipeline;
        m_RenderPipeline = nullptr;

        delete m_DefaultTexture;
        m_DefaultTexture = nullptr;

        clearRenderAPIObject();

        Super::clearSubsystemInternal();
    }

    Shader_RenderAPIObject* RenderSubsystem::createShaderObject()
    {
        RenderAPIObjectType* renderObject = getRenderAPIObject();
        return renderObject != nullptr ? renderObject->createShaderObject() : nullptr;
    }
    Material_RenderAPIObject* RenderSubsystem::createMaterialObject()
    {
        RenderAPIObjectType* renderObject = getRenderAPIObject();
        return renderObject != nullptr ? renderObject->createMaterialObject() : nullptr;
    }
    VertexBuffer_RenderAPIObject* RenderSubsystem::createVertexBufferObject()
    {
        RenderAPIObjectType* renderObject = getRenderAPIObject();
        return renderObject != nullptr ? renderObject->createVertexBufferObject() : nullptr;
    }
    Texture_RenderAPIObject* RenderSubsystem::createTextureObject()
    {
        RenderAPIObjectType* renderObject = getRenderAPIObject();
        return renderObject != nullptr ? renderObject->createTextureObject() : nullptr;
    }
    RenderTarget_RenderAPIObject* RenderSubsystem::createRenderTargetObject()
    {
        RenderAPIObjectType* renderObject = getRenderAPIObject();
        return renderObject != nullptr ? renderObject->createRenderTargetObject() : nullptr;
    }
    RenderPipeline_RenderAPIObject* RenderSubsystem::createRenderPipelineObject()
    {
        RenderAPIObjectType* renderObject = getRenderAPIObject();
        return renderObject != nullptr ? renderObject->createRenderPipelineObject() : nullptr;
    }

    void RenderSubsystem::render()
    {
        WindowSubsystem* windowSubsystem = getOwnerEngine()->getWindowSubsystem();
        if ((m_RenderPipeline == nullptr) || (windowSubsystem == nullptr))
        {
            return;
        }

        RenderSubsystem_RenderAPIObject* renderObject = getRenderAPIObject();
        renderObject->onStartRender();
        windowSubsystem->onStartRender();
        m_RenderPipeline->render();
        windowSubsystem->onFinishRender();
        renderObject->onFinishRender();
    }
    void RenderSubsystem::waitForRenderFinish()
    {
        RenderAPIObjectType* renderObject = getRenderAPIObject();
        if (renderObject != nullptr)
        {
            renderObject->waitForRenderFinish();
        }
    }
}
