// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "MeshFileImporter_Assimp.h"

namespace JumaEngine
{
    void ReadSceneMesh(MeshFileImporterMeshPartData& data, const aiScene* scene, const uint32 meshID)
    {
        if (meshID >= scene->mNumMeshes)
        {
            return;
        }

        const aiMesh* mesh = scene->mMeshes[meshID];
        data.verticesData.clear();
        for (uint32 i = 0; i < mesh->mNumVertices; i++)
        {
            MeshFileImporterVertexData& vertexData = data.verticesData.addDefault();

            const aiVector3D position = mesh->mVertices[i];
            vertexData.position = { position.x, position.y, position.z };

            if (mesh->mNormals != nullptr)
            {
                const aiVector3D normal = mesh->mNormals[i];
                vertexData.normal = { normal.x, normal.y, normal.z };
            }
        }
    }

    void ReadSceneNode(MeshFileImporterData& data, const aiScene* scene, const aiNode* node, const aiMatrix4x4& parentTransform)
    {
        if (node == nullptr)
        {
            return;
        }

        const aiMatrix4x4 transform = node->mTransformation * parentTransform;
        if (node->mNumMeshes > 0)
        {
            MeshFileImporterMeshData& meshData = data.meshesData.addDefault();
            meshData.name = node->mName.C_Str();

            aiVector3D location;
            aiQuaternion rotationQuat;
            aiVector3D scale;
            transform.Decompose(scale, rotationQuat, location);

            Rotation rotation;
            rotation.fromQuat({ rotationQuat.w, rotationQuat.x, rotationQuat.y, rotationQuat.z });
            meshData.transform = Transform({ location.x, location.y, location.z }, rotation, { scale.x, scale.y, scale.z });

            for (uint32 i = 0; i < node->mNumMeshes; i++)
            {
                const uint32 meshID = node->mMeshes[i];
                MeshFileImporterMeshPartData& meshPartData = meshData.meshPartsData.addDefault();
                ReadSceneMesh(meshPartData, scene, meshID);
            }
        }

        for (uint32 i = 0; i < node->mNumChildren; i++)
        {
            ReadSceneNode(data, scene, node->mChildren[i], transform);
        }
    }

    void MeshFileImporter_Assimp::importMeshFile(const char* filePath)
    {
        Assimp::Importer importer;
        importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_COLORS | aiComponent_LIGHTS | aiComponent_CAMERAS);

        const aiScene* scene = importer.ReadFile(
            filePath, 
            aiProcess_Triangulate | aiProcess_RemoveComponent | aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph
        );
        if (scene != nullptr)
        {
            ReadSceneNode(m_Data, scene, scene->mRootNode, aiMatrix4x4());
            importer.FreeScene();
        }
    }
}
