// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "MaterialBase.h"
#include "render/shader/ShaderBase.h"

namespace JumaEngine
{
	void MaterialBase::loadMaterialParams(const window_id windowID) const
	{
		const jmap<jstring, MaterialParamType>& paramsList = getMaterialParamsList();
		for (const auto& paramNameAndType : paramsList)
		{
			const jstring& name = paramNameAndType.first;
			switch (paramNameAndType.second)
			{
			case MaterialParamType::Bool:
				{
					bool value;
					if (getMaterialParam(name, value))
					{
						ShaderBase::setActiveShaderUniformValue(windowID, *name, value);
					}
				}
				break;
				
			case MaterialParamType::Int:
				{
					int32 value;
					if (getMaterialParam(name, value))
					{
						ShaderBase::setActiveShaderUniformValue(windowID, *name, value);
					}
				}
				break;
				
			case MaterialParamType::Float:
				{
					float value;
					if (getMaterialParam(name, value))
					{
						ShaderBase::setActiveShaderUniformValue(windowID, *name, value);
					}
				}
				break;
				
			case MaterialParamType::Vec2:
				{
					glm::vec2 value;
					if (getMaterialParam(name, value))
					{
						ShaderBase::setActiveShaderUniformValue(windowID, *name, value);
					}
				}
				break;
				
			case MaterialParamType::Vec3:
				{
					glm::vec3 value;
					if (getMaterialParam(name, value))
					{
						ShaderBase::setActiveShaderUniformValue(windowID, *name, value);
					}
				}
				break;
				
			case MaterialParamType::Vec4:
				{
					glm::vec4 value;
					if (getMaterialParam(name, value))
					{
						ShaderBase::setActiveShaderUniformValue(windowID, *name, value);
					}
				}
				break;
				
			case MaterialParamType::Mat4:
				{
					glm::mat4 value;
					if (getMaterialParam(name, value))
					{
						ShaderBase::setActiveShaderUniformValue(windowID, *name, value);
					}
				}
				break;
				
			default: ;
			}
		}
	}
}
