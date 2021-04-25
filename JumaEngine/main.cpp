// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Engine.h"
#include "render/vertexBuffer/importer/VertexBufferImporterBase.h"

int main(int argc, char** argv)
{
    JumaEngine::Engine engine;
    engine.setVertexBufferImporter<JumaEngine::VertexBufferImporterBase>();
    return engine.startEngine(argc, argv);
}
