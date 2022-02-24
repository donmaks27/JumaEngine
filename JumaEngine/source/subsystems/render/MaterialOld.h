// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "engine/EngineContextObject.h"

#include "MaterialUniform.h"
#include "jutils/jshared_ptr.h"
#include "jutils/jdelegate_multicast.h"

namespace JumaEngine
{
    class ShaderOld;
    class MaterialOld;

    CREATE_JUTILS_MULTICAST_DELEGATE_OneParam(OnMaterialOldEvent, MaterialOld*, material);

    class MaterialOld : public EngineContextObject
    {
        JUMAENGINE_ABSTRACT_CLASS(MaterialOld, EngineContextObject)

    public:
        MaterialOld() = default;
        virtual ~MaterialOld() override;

        OnMaterialOldEvent onPreClear;
        OnMaterialOldEvent onCleared;


        bool isValid() const;
        bool init(const jshared_ptr<ShaderOld>& shader);
        bool init(const jshared_ptr<MaterialOld>& material);
        void clear();

        bool isMaterialInstance() const { return m_BaseMaterial != nullptr; }
        const jshared_ptr<ShaderOld>& getShader() const { return isMaterialInstance() ? m_BaseMaterial->getShader() : m_BaseShader; }

        const jarray<jstring>& getUniformNames() const { return isMaterialInstance() ? m_BaseMaterial->getUniformNames() : m_UniformNames; }

        ShaderUniformType getUniformType(const jstring& name) const;
        template<ShaderUniformType Type>
        bool getUniformValue(const jstring& name, typename MaterialUniformType<Type>::struct_type::value_type& outValue) const
        {
            return MaterialUniformActions::get<Type>(getUniformValue(name), outValue);
        }
        template<ShaderUniformType Type>
        bool setUniformValue(const jstring& name, const typename MaterialUniformType<Type>::struct_type::value_type& value)
        {
            if (isValid() && (getUniformType(name) == Type))
            {
                const int32 index = m_UniformNames.indexOf(name);
                if (index != -1)
                {
                    if (!MaterialUniformActions::set<Type>(m_UniformValues[index], value))
                    {
                        return false;
                    }
                }
                else
                {
                    MaterialUniform* uniform = MaterialUniformActions::create<Type>(value);
                    if (uniform == nullptr)
                    {
                        return false;
                    }

                    m_UniformNames.add(name);
                    m_UniformValues.add(uniform);
                }
                onMaterialUniformChanged(name);
                return true;
            }
            return false;
        }

    protected:

        jarray<jstring> m_UniformNames;
        jarray<MaterialUniform*> m_UniformValues;


        virtual bool initInternal(const jshared_ptr<ShaderOld>& shader) = 0;
        virtual void clearInternal() = 0;

        virtual void onMaterialUniformChanged(const jstring& name) {}

        void onMaterialPreClear() { onPreClear.call(this); }
        void onMaterialCleared() { onCleared.call(this); }
        
        MaterialUniform* getUniformValue(const jstring& name) const;
        
    private:

        jshared_ptr<ShaderOld> m_BaseShader = nullptr;
        jshared_ptr<MaterialOld> m_BaseMaterial = nullptr;


        void initUniformValues(const jshared_ptr<ShaderOld>& shader);

        void onBaseShaderPreClear(ShaderOld* shader) { clear(); }
        void onBaseMaterialPreClear(MaterialOld* material) { clear(); }

        void clearMaterial();
        void clearUniformValues();
    };
}
