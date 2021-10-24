// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "engine/Engine.h"
#include "asset/mesh/MeshFileImporter_Assimp.h"
#include "asset/texture/TextureFileImporter_DevIL.h"

int main(int argc, char** argv)
{
    JumaEngine::EngineOld engine;
    engine.setMeshFileImporter<JumaEngine::MeshFileImporter_Assimp>();
    engine.setTextureFileImporter<JumaEngine::TextureFileImporter_DevIL>();
    return engine.startEngine(argc, argv);
}
