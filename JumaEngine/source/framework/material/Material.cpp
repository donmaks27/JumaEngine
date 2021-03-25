﻿// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Material.h"
#include "utils/system_functions.h"
#include "framework/gameObject/Camera.h"
#include "render/shader/ShaderBase.h"

namespace JumaEngine
{
    void Material::loadShaderParams() const
    {
        const Camera* camera = SystemFunctions::getActiveCamera(this);
        if (camera != nullptr)
        {
            ShaderBase::setActiveShaderUniformValue("uProjection", camera->getProjectionMatrix());
            ShaderBase::setActiveShaderUniformValue("uView", camera->getViewMatrix());
        }
        ShaderBase::setActiveShaderUniformValue("uModel", m_ModelMatrix);
    }
}