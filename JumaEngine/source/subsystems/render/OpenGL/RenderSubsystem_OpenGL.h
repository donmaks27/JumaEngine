// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/RenderSubsystem.h"

#include "OpenGLContextObject.h"
#include "jutils/jdelegate_multicast.h"
#include "jutils/jset.h"

namespace JumaEngine
{
    class WindowSubsystem_RenderAPIObject_OpenGL;

    class RenderSubsystem_RenderAPIObject_OpenGL : public RenderSubsystem_RenderAPIObject
    {
        using Super = RenderSubsystem_RenderAPIObject;

    public:
        RenderSubsystem_RenderAPIObject_OpenGL() = default;
        virtual ~RenderSubsystem_RenderAPIObject_OpenGL() override;

        template<typename T, TEMPLATE_ENABLE(is_base_and_not_same<OpenGLContextObject, T>)>
        T* createOpenGLObject() { return this->registerOpenGLObject(new T()); }
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_same<OpenGLContextObject, T>)>
        T* registerOpenGLObject(T* openGLContextObject)
        {
            if (openGLContextObject != nullptr)
            {
                openGLContextObject->m_RenderSubsystem = this;
                openGLContextObject->m_WindowSubsystem = m_WindowSubsystem;
            }
            return openGLContextObject;
        }

        void flushObjectsChanges();

    protected:

        virtual bool initInternal() override;

        virtual Shader_RenderAPIObject* createShaderObject() override;
        virtual Material_RenderAPIObject* createMaterialObject() override;
        virtual VertexBuffer_RenderAPIObject* createVertexBufferObject() override;
        virtual Texture_RenderAPIObject* createTextureObject() override;
        virtual RenderTarget_RenderAPIObject* createRenderTargetObject() override;
        virtual RenderPipeline_RenderAPIObject* createRenderPipelineObject() override;

        virtual void onFinishRender() override;

    private:

        WindowSubsystem_RenderAPIObject_OpenGL* m_WindowSubsystem = nullptr;

        jarray<OpenGLContextObject*> m_CreatedObjects;
        jarray<OpenGLContextObject*> m_ObjectsForFlush;


        void clearOpenGL();

        void onObjectCreated(OpenGLContextObject* object);
    };
}

#endif
