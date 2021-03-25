// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Engine.h"
#include "window/WindowGLFW.h"
#include "render/RenderManagerOpenGL.h"
#include "render/vertexBuffer/importer/VertexBufferImporterBase.h"

int main(int argc, char** argv)
{
    JumaEngine::Engine engine;
    engine.setWindow<JumaEngine::WindowGLFW>();
    engine.setRenderManager<JumaEngine::RenderManagerOpenGL>();
    engine.setVertexBufferImporter<JumaEngine::VertexBufferImporterBase>();
    return engine.startEngine(argc, argv);
}
