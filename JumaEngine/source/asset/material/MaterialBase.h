// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "utils/jmap.h"
#include "glm/mat4x4.hpp"
#include "render/texture/TextureShaderUniform.h"
#include "asset/AssetObject.h"

namespace JumaEngine
{
    class TextureBase;
    class ShaderBase;

	class MaterialBase : public AssetObject
	{
        JUMAENGINE_CLASS(MaterialBase, AssetObject)

	public:
		MaterialBase() = default;
		virtual ~MaterialBase() override = default;

		template<typename T>
		bool hasMaterialParam(const jstring& name) const
		{
			const MaterialParamType* typePtr = m_MaterialParams.findByKey(name);
			if (typePtr != nullptr)
			{
				return getMaterialParamType<T>() == *typePtr;
			}

			const MaterialBase* baseMaterial = getBaseMaterial();
			if (baseMaterial != nullptr)
			{
				return baseMaterial->hasMaterialParam<T>(name);
			}

			return false;
		}
		template<typename T, typename U = T>
		const U* findMaterialParam(const jstring& name) const
		{
		    const MaterialParamType* typePtr = m_MaterialParams.findByKey(name);
			if (typePtr != nullptr)
			{
				if (getMaterialParamType<T>() == *typePtr)
				{
					return getMaterialParams<T, U>().findByKey(name);
				}
			}
			else
			{
				const MaterialBase* baseMaterial = getBaseMaterial();
				if (baseMaterial != nullptr)
				{
                    return baseMaterial->findMaterialParam<T, U>(name);
				}
			}
			return nullptr;
		}
		
		template<typename T>
		bool setMaterialParam(const jstring& name, const T& value)
		{
			if (hasMaterialParam<T>(name))
			{
				m_MaterialParams.add(name, getMaterialParamType<T>());
				getMaterialParams<T>().add(name, value);
				return true;
			}
			return false;
		}
        
        virtual ShaderBase* getShader() const = 0;

		virtual bool isInit() const = 0;
        virtual bool activate() const = 0;
        virtual void deactivate() const = 0;
		
		void loadMaterialParams() const;

	protected:

		virtual MaterialBase* getBaseMaterial() const = 0;
		
		template<typename T, typename U = T>
		bool addMaterialParamInternal(const jstring& name, const U& value)
		{
			if (!m_MaterialParams.contains(name))
			{
				m_MaterialParams.add(name, getMaterialParamType<T>());
				getMaterialParams<T, U>().add(name, value);
				return true;
			}
			return false;
		}

	private:

		enum class MaterialParamType
		{
			Bool, Int, Float, Vec2, Vec3, Vec4, Mat4, Texture
		};
		
		jmap<jstring, MaterialParamType> m_MaterialParams;
		jmap<jstring, bool> m_MaterialParamsBool;
		jmap<jstring, int32> m_MaterialParamsInt;
		jmap<jstring, float> m_MaterialParamsFloat;
		jmap<jstring, glm::vec2> m_MaterialParamsVec2;
		jmap<jstring, glm::vec3> m_MaterialParamsVec3;
		jmap<jstring, glm::vec4> m_MaterialParamsVec4;
		jmap<jstring, glm::mat4> m_MaterialParamsMat4;
		jmap<jstring, TextureShaderUniform> m_MaterialParamsTexture;


		jmap<jstring, bool>& getMaterialParams(const bool*) { return m_MaterialParamsBool; }
		const jmap<jstring, bool>& getMaterialParams(const bool*) const { return m_MaterialParamsBool; }
		jmap<jstring, int32>& getMaterialParams(const int32*) { return m_MaterialParamsInt; }
		const jmap<jstring, int32>& getMaterialParams(const int32*) const { return m_MaterialParamsInt; }
		jmap<jstring, float>& getMaterialParams(const float*) { return m_MaterialParamsFloat; }
		const jmap<jstring, float>& getMaterialParams(const float*) const { return m_MaterialParamsFloat; }
		jmap<jstring, glm::vec2>& getMaterialParams(const glm::vec2*) { return m_MaterialParamsVec2; }
		const jmap<jstring, glm::vec2>& getMaterialParams(const glm::vec2*) const { return m_MaterialParamsVec2; }
		jmap<jstring, glm::vec3>& getMaterialParams(const glm::vec3*) { return m_MaterialParamsVec3; }
		const jmap<jstring, glm::vec3>& getMaterialParams(const glm::vec3*) const { return m_MaterialParamsVec3; }
		jmap<jstring, glm::vec4>& getMaterialParams(const glm::vec4*) { return m_MaterialParamsVec4; }
		const jmap<jstring, glm::vec4>& getMaterialParams(const glm::vec4*) const { return m_MaterialParamsVec4; }
		jmap<jstring, glm::mat4>& getMaterialParams(const glm::mat4*) { return m_MaterialParamsMat4; }
		const jmap<jstring, glm::mat4>& getMaterialParams(const glm::mat4*) const { return m_MaterialParamsMat4; }
		jmap<jstring, TextureShaderUniform>& getMaterialParams(const TextureBase*) { return m_MaterialParamsTexture; }
		const jmap<jstring, TextureShaderUniform>& getMaterialParams(const TextureBase*) const { return m_MaterialParamsTexture; }
		template<typename T, typename U = T>
		jmap<jstring, U>& getMaterialParams() { return getMaterialParams(static_cast<T*>(nullptr)); }
		template<typename T, typename U = T>
		const jmap<jstring, U>& getMaterialParams() const { return getMaterialParams(static_cast<T*>(nullptr)); }

		static MaterialParamType getMaterialParamType(const bool*) { return MaterialParamType::Bool; }
		static MaterialParamType getMaterialParamType(const int32*) { return MaterialParamType::Int; }
		static MaterialParamType getMaterialParamType(const float*) { return MaterialParamType::Float; }
		static MaterialParamType getMaterialParamType(const glm::vec2*) { return MaterialParamType::Vec2; }
		static MaterialParamType getMaterialParamType(const glm::vec3*) { return MaterialParamType::Vec3; }
		static MaterialParamType getMaterialParamType(const glm::vec4*) { return MaterialParamType::Vec4; }
		static MaterialParamType getMaterialParamType(const glm::mat4*) { return MaterialParamType::Mat4; }
		static MaterialParamType getMaterialParamType(const TextureBase*) { return MaterialParamType::Texture; }
		template<typename T>
		static MaterialParamType getMaterialParamType() { return getMaterialParamType(static_cast<T*>(nullptr)); }

		const jmap<jstring, MaterialParamType>& getMaterialParamsList() const
		{
			const MaterialBase* baseMaterial = getBaseMaterial();
			return baseMaterial != nullptr ? baseMaterial->getMaterialParamsList() : m_MaterialParams;
		}
	};
}
