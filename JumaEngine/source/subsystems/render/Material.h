// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "MaterialUniform.h"
#include "RenderInterface.h"
#include "engine/EngineContextObject.h"
#include "utils/jshared_ptr.h"
#include "utils/jdelegate_multicast.h"
#include "utils/jmap.h"

namespace JumaEngine
{
    class Shader;
    class Material;

    CREATE_JDELEGATE_MULTICAST_TYPE_OneParam(OnMaterialEvent, Material*, material)

    class Material : public EngineContextObject, public IRenderActivateInterface
    {
        JUMAENGINE_CLASS(Material, EngineContextObject)

    public:
        Material() = default;
        virtual ~Material() override;

        OnMaterialEvent onPreClear;
        OnMaterialEvent onCleared;


        bool isValid() const;
        bool init(const jshared_ptr<Shader>& shader);
        bool init(const jshared_ptr<Material>& material);
        void clear();

        bool isMaterialInstance() const { return m_BaseMaterial != nullptr; }
        const jshared_ptr<Shader>& getShader() const { return isMaterialInstance() ? m_BaseMaterial->getShader() : m_BaseShader; }

        const jarray<jstring>& getUniformNames() const { return isMaterialInstance() ? m_BaseMaterial->getUniformNames() : m_UniformNames; }

        ShaderUniformType getUniformType(const jstring& name) const;
        template<typename T, TEMPLATE_ENABLE(is_shader_uniform_type<T>)>
        bool getUniformValue(const jstring& name, T& outValue) const
        {
            const MaterialUniform* uniform = getUniformValue(name);
            return (uniform != nullptr) && MaterialUniformActions::get(uniform, outValue);
        }
        template<typename T, TEMPLATE_ENABLE(is_shader_uniform_type<T>)>
        bool setUniformValue(const jstring& name, const T& value)
        {
            if (isValid() && (getUniformType(name) == MaterialUniformActions::getType<T>()))
            {
                bool result = false;
                const int64 index = m_UniformNames.indexOf(name);
                if (index != -1)
                {
                    result = MaterialUniformActions::set(m_UniformValues[index], value);
                }
                else if (isMaterialInstance())
                {
                    MaterialUniform* uniform = MaterialUniformActions::create(value);
                    if (uniform != nullptr)
                    {
                        m_UniformNames.add(name);
                        m_UniformValues.add(uniform);
                        result = true;
                    }
                }
                if (result)
                {
                    onMaterialUniformChanged(name);
                    return true;
                }
            }
            return false;
        }

    protected:

        virtual bool initInternal(const jshared_ptr<Shader>& shader) = 0;
        virtual void clearInternal() = 0;

        virtual void onMaterialUniformChanged(const jstring& name) {}

        void onMaterialPreClear() { onPreClear.call(this); }
        void onMaterialCleared() { onCleared.call(this); }
        
        MaterialUniform* getUniformValue(const jstring& name) const;
        
    private:

        jshared_ptr<Shader> m_BaseShader = nullptr;
        jshared_ptr<Material> m_BaseMaterial = nullptr;

        jarray<jstring> m_UniformNames;
        jarray<MaterialUniform*> m_UniformValues;


        void initUniformValues(const jshared_ptr<Shader>& shader);

        void onBaseShaderPreClear(Shader* shader) { clear(); }
        void onBaseMaterialPreClear(Material* material) { clear(); }

        void clearMaterial();
        void clearUniformValues();
    };
}
