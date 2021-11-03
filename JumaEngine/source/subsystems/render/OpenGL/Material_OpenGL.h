// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/Material.h"

namespace JumaEngine
{
    class Material_OpenGL final : public Material
    {
        JUMAENGINE_CLASS(Material_OpenGL, Material)

    public:
        Material_OpenGL() = default;
        virtual ~Material_OpenGL() override;
        
        bool activate();
        void deactivate();

    protected:

        virtual bool initInternal(const jshared_ptr<Shader>& shader) override;
        virtual void clearInternal() override;

    private:

        uint32 m_ActivatedImagesCount = 0;
    };
}

#endif
