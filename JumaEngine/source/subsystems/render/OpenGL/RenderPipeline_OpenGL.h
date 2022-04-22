// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/RenderPipeline.h"
#include "OpenGLContextObject.h"

#include "subsystems/asyncTasks/ActionTask.h"

namespace JumaEngine
{
    struct RenderOptions_OpenGL;
    class WindowSubsystem;
    class RenderTarget_RenderAPIObject_OpenGL;

    class RenderPipeline_RenderAPIObject_OpenGL : public RenderPipeline_RenderAPIObject, public OpenGLContextObject
    {
    public:
        RenderPipeline_RenderAPIObject_OpenGL() = default;
        virtual ~RenderPipeline_RenderAPIObject_OpenGL() override;

    protected:

        virtual bool initInternal() override;
        
        virtual bool renderPipeline() override;

    private:

        void clearData();

        bool callRenderForRenderTarget(RenderTarget_RenderAPIObject_OpenGL* renderTargetObject, 
            const jmap<jstringID, const ActionTaskResult<bool>*>* renderTaskResults, RenderOptions_OpenGL options);
    };
}

#endif
