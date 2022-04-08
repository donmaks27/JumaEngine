// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderAPIObject.h"
#include "engine/EngineContextObject.h"

#include "jutils/jmap.h"
#include "material/ShaderUniform.h"
#include "jutils/jstringID.h"
#include "jutils/jdelegate_multicast.h"

namespace JumaEngine
{
    class Shader;

    class Shader_RenderAPIObject : public RenderAPIObject<Shader>
    {
    public:
        Shader_RenderAPIObject() = default;
        virtual ~Shader_RenderAPIObject() override = default;
    };

    CREATE_JUTILS_MULTICAST_DELEGATE_OneParam(OnShaderEvent, Shader*, shader);

    class Shader final : public EngineContextObject, public RenderAPIWrapper<Shader_RenderAPIObject>
    {
        JUMAENGINE_CLASS(Shader, EngineContextObject)

    public:
        Shader() = default;
        virtual ~Shader() override;

        OnShaderEvent onClear;


        bool init(const jstring& vertexShader, const jstring& fragmentShader, jmap<jstringID, ShaderUniform> uniforms = {});

        jstring getVertexShaderName() const { return m_VertexShaderName; }
        jstring getFragmentShaderName() const { return m_FragmentShaderName; }
        const jmap<jstringID, ShaderUniform>& getUniforms() const { return m_ShaderUniforms; }

    protected:

        virtual Shader_RenderAPIObject* createRenderAPIObjectInternal() override;

        virtual void clearInternal() override { clearData(); }

    private:

        jstring m_VertexShaderName;
        jstring m_FragmentShaderName;
        jmap<jstringID, ShaderUniform> m_ShaderUniforms;


        void clearData();
    };
}
