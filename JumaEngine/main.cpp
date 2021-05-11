// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "engine/Engine.h"
#include "asset/mesh/MeshFileImporter_Assimp.h"

int main(int argc, char** argv)
{
    JumaEngine::Engine engine;
    engine.setVertexBufferImporter<JumaEngine::MeshFileImporter_Assimp>();
    return engine.startEngine(argc, argv);
}
