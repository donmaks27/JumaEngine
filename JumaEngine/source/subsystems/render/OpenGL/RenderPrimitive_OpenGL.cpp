// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "RenderPrimitive_OpenGL.h"

#if defined(JUMAENGINE_INCLUDE_RENDER_API_OPENGL)

#include <GL/glew.h>

#include "Material_OpenGL.h"
#include "VertexBuffer_OpenGL.h"

namespace JumaEngine
{
    void RenderPrimitive_OpenGL::render(const RenderOptionsOld& options)
    {
        if (!isValid())
        {
            return;
        }

        Material_OpenGL* material = dynamic_cast<Material_OpenGL*>(getMaterial().get());
        if (material->activate())
        {
            if (options.data->invertFacesOrientation)
            {
                glCullFace(GL_FRONT);
            }
            else
            {
                glCullFace(GL_BACK);
            }
            dynamic_cast<VertexBuffer_OpenGL*>(getVertexBuffer().get())->render();
            material->deactivate();
        }
    }
}

#endif
