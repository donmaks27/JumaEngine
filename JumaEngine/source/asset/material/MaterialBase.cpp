// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "MaterialBase.h"
#include "render/shader/ShaderBase.h"

namespace JumaEngine
{
    template<typename T>
    void loadMaterialParam(const MaterialBase* material, ShaderBase* shader, const jstring& name)
    {
        T value = T();
        if (material->getMaterialParam(name, value))
        {
            shader->setUniformValue(*name, value);
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
				
			default: ;
			}
		}
	}
}
