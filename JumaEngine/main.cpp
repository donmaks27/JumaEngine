// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Engine.h"
#include "render/vertexBuffer/importer/MeshImporterBase.h"

int main(int argc, char** argv)
{
    JumaEngine::Engine engine;
    engine.setVertexBufferImporter<JumaEngine::MeshImporterBase>();
    return engine.startEngine(argc, argv);
}
