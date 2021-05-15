// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "MaterialBase.h"
#include "render/shader/ShaderBase.h"

namespace JumaEngine
{
    template<typename T>
    void loadMaterialParam(const MaterialBase* material, ShaderBase* shader, const jstring& name, const uint32 index = 0)
    {
        const T* valuePtr = material->findMaterialParam<T>(name);
        if (valuePtr != nullptr)
        {
            shader->setUniformValue(*name, *valuePtr);
        }
    }
    template<>
    void loadMaterialParam<TextureBase>(const MaterialBase* material, ShaderBase* shader, const jstring& name, const uint32 index)
    {
        const asset_ptr<TextureBase>* texture = material->findMaterialParam<TextureBase, asset_ptr<TextureBase>>(name);
        if (texture != nullptr)
        {
            shader->setUniformValue(*name, ShaderUniformTexture{ texture, index });
        }
    }

	void MaterialBase::loadMaterialParams() const
	{
        ShaderBase* shader = getShader();
        if (shader == nullptr)
        {
            return;
        }

		const jmap<jstring, MaterialParamType>& paramsList = getMaterialParamsList();
        int32 nextTextureIndex = 0;
		for (const auto& paramNameAndType : paramsList)
		{
			const jstring& name = paramNameAndType.first;
			switch (paramNameAndType.second)
			{
			case MaterialParamType::Bool:
				loadMaterialParam<bool>(this, shader, name);
				break;
				
			case MaterialParamType::Int:
                loadMaterialParam<int32>(this, shader, name);
				break;
				
			case MaterialParamType::Float:
                loadMaterialParam<float>(this, shader, name);
				break;
				
			case MaterialParamType::Vec2:
                loadMaterialParam<glm::vec2>(this, shader, name);
				break;
				
			case MaterialParamType::Vec3:
                loadMaterialParam<glm::vec3>(this, shader, name);
				break;
				
			case MaterialParamType::Vec4:
                loadMaterialParam<glm::vec4>(this, shader, name);
				break;
				
			case MaterialParamType::Mat4:
                loadMaterialParam<glm::mat4>(this, shader, name);
				break;

            case MaterialParamType::Texture: 
                loadMaterialParam<TextureBase>(this, shader, name, nextTextureIndex);
                ++nextTextureIndex;
                break;

			default: ;
			}
		}
	}
}
