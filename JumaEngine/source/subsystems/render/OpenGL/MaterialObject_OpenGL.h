// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include "subsystems/render/Material.h"

namespace JumaEngine
{
    class MaterialObject_OpenGL : public MaterialObject
    {
    public:
        MaterialObject_OpenGL() = default;
        virtual ~MaterialObject_OpenGL() override = default;

        virtual bool render(VertexBuffer* vertexBuffer, const RenderOptions* renderOptions) override;

    protected:

        virtual bool initInternal() override { return true; }
        virtual void clearInternal() override {}

    private:

        uint32 m_ActivatedImageCount = 0;


        bool activate();
        void deactivate();
    };
}

#endif
