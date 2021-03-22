// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "MaterialBase.h"
#include "glm/mat4x4.hpp"

namespace JumaEngine
{
    class Material : public MaterialBase
    {
    public:
        Material() = default;
        virtual ~Material() override = default;

        void setModelMatrix(const glm::mat4& matrix) { m_ModelMatrix = matrix; }

    protected:

        virtual void loadShaderParams() const override;

    private:

        glm::mat4 m_ModelMatrix = glm::mat4(1.0f);
    };
}
