// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "ShaderUniform.h"
#include "engine/EngineContextObject.h"
#include "utils/jarray.h"

namespace JumaEngine
{
    class Shader : public EngineContextObject
    {
        JUMAENGINE_CLASS(Shader, EngineContextObject)

    public:
        Shader() = default;
        virtual ~Shader() override = default;

        bool init(const jstring& shaderName, const jarray<ShaderUniform>& uniforms = {});
        bool isValid() const { return m_Initialized; }
        void clear();

        jstring getName() const { return isValid() ? m_Name : JSTR(""); }
        const jarray<ShaderUniform>& getUniforms() const { return m_ShaderUniforms; }

    protected:

        virtual bool initInternal(const jstring& shaderName, const jarray<ShaderUniform>& uniforms) = 0;
        virtual void clearInternal() = 0;

    private:

        bool m_Initialized = false;
        jstring m_Name;

        jarray<ShaderUniform> m_ShaderUniforms;
    };
}
