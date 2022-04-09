// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/Shader.h"

namespace JumaEngine
{
    class Shader_RenderAPIObject_OpenGL : public Shader_RenderAPIObject
    {
    public:
        Shader_RenderAPIObject_OpenGL() = default;
        virtual ~Shader_RenderAPIObject_OpenGL() override;

        bool activate() const;
        static void deactivate();

    protected:

        virtual bool initInternal() override;

    private:

        uint32 m_ShaderProgramIndex = 0;


        uint32 compileShader(const jstring& fileName, ShaderStage stage, bool optional = false) const;
        uint32 compileShaderProgram(const jarray<uint32>& shaders) const;

        void clearOpenGL();
    };
}

#endif
