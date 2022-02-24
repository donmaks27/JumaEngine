// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderObject.h"
#include "engine/EngineContextObject.h"

#include "jutils/jmap.h"
#include "ShaderUniform.h"
#include "jutils/jstringID.h"
#include "jutils/jdelegate_multicast.h"

namespace JumaEngine
{
    class Shader;

    class ShaderObject : public RenderObject<Shader>
    {
    public:
        ShaderObject() = default;
        virtual ~ShaderObject() override = default;
    };

    CREATE_JUTILS_MULTICAST_DELEGATE_OneParam(OnShaderEvent, Shader*, shader);

    class Shader final : public EngineContextObject
    {
        JUMAENGINE_CLASS(Shader, EngineContextObject)

    public:
        Shader() = default;
        virtual ~Shader() override;

        OnShaderEvent onClear;


        bool init(const jstring& vertexShader, const jstring& fragmentShader, jmap<jstringID, ShaderUniform> uniforms = {});
        bool isValid() const { return m_Initialized; }
        void clear();

        jstring getVertexShaderName() const { return m_VertexShaderName; }
        jstring getFragmentShaderName() const { return m_FragmentShaderName; }
        const jmap<jstringID, ShaderUniform>& getUniforms() const { return m_ShaderUniforms; }

        bool createRenderObject();
        ShaderObject* getRenderObject() const { return m_RenderObject; }
        void clearRenderObject();

    private:

        bool m_Initialized = false;

        jstring m_VertexShaderName;
        jstring m_FragmentShaderName;
        jmap<jstringID, ShaderUniform> m_ShaderUniforms;

        ShaderObject* m_RenderObject = nullptr;
    };
}
