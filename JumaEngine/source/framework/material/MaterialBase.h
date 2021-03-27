// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "EngineContextObject.h"

namespace JumaEngine
{
    class ShaderBase;

    class MaterialBase : public EngineContextObject
    {
    public:
        MaterialBase() = default;
        virtual ~MaterialBase() override;

        bool isShaderValid() const;
        jstring getShaderName() const;
        void setShaderName(const jstring& shaderName);
        void clearShader();

        void activate() const;
        bool isActive() const;
        void deactivate() const;

    protected:

        virtual void loadShaderParams() const = 0;

    private:

        ShaderBase* m_Shader = nullptr;
    };
}
