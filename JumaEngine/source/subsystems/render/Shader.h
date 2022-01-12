// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "engine/EngineContextObject.h"

#include "ShaderUniform.h"
#include "jutils/jmap.h"
#include "jutils/jdelegate_multicast.h"

namespace JumaEngine
{
    class Shader;

    CREATE_JUTILS_MULTICAST_DELEGATE_OneParam(OnShaderEvent, Shader*, shader);

    class Shader : public EngineContextObject
    {
        JUMAENGINE_ABSTRACT_CLASS(Shader, EngineContextObject)

    public:
        Shader() = default;
        virtual ~Shader() override;

        OnShaderEvent onPreClear;
        OnShaderEvent onCleared;


        bool init(const jstring& shaderName, const jmap<jstring, ShaderUniform>& uniforms = {});
        bool isValid() const { return m_Initialized; }
        void clear();

        jstring getName() const { return isValid() ? m_Name : JSTR(""); }
        const jmap<jstring, ShaderUniform>& getUniforms() const { return m_ShaderUniforms; }

    protected:

        virtual bool initInternal(const jstring& shaderName, const jmap<jstring, ShaderUniform>& uniforms) = 0;
        virtual void clearInternal() = 0;

        void onShaderPreClear() { onPreClear.call(this); }
        void onShaderCleared() { onCleared.call(this); }

    private:

        bool m_Initialized = false;
        jstring m_Name;

        jmap<jstring, ShaderUniform> m_ShaderUniforms;
    };
}
