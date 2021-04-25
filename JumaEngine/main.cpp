// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Engine.h"
#include "render/RenderManager_OpenGL_GLFW.h"
#include "render/vertexBuffer/importer/VertexBufferImporterBase.h"

int main(int argc, char** argv)
{
    JumaEngine::Engine engine;
    engine.setRenderManager<JumaEngine::RenderManager_OpenGL_GLFW>();
    engine.setVertexBufferImporter<JumaEngine::VertexBufferImporterBase>();
    return engine.startEngine(argc, argv);
}
