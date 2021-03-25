// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "VertexBufferImporterData.h"
#include "render/vertexBuffer/VertexPosition.h"
#include "utils/type_traits_macros.h"

namespace JumaEngine
{
    class VertexBufferImporterBase
    {
    public:
        VertexBufferImporterBase() = default;
        virtual ~VertexBufferImporterBase() = default;

        void load(const char* filePath);

        std::vector<std::string> getMeshesNames() const;
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<VertexBufferDataBase, T>)>
        std::vector<VertexBufferDataBase*> createVertexBufferForMesh(const std::string& meshName)
        {
            std::vector<VertexBufferDataBase*> result;
            for (const auto& meshData : m_Data.meshesData)
            {
                if (meshData.name == meshName)
                {
                    for (const auto& meshPartData : meshData.meshPartsData)
                    {
                        VertexBufferDataBase* vertexBufferData = new T();
                        vertexBufferData->copyFromVertexBufferImporterData(meshPartData);
                        result.push_back(vertexBufferData);
                    }
                    break;
                }
            }
            return result;
        }

    protected:

        virtual VertexBufferImporterData importFromFile(const char* filePath) const;

    private:

        VertexBufferImporterData m_Data;
    };
}
