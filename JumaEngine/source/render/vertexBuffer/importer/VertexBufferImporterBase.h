// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "VertexBufferImporterData.h"
#include "render/vertexBuffer/VertexBufferData.h"
#include "utils/jarray.h"
#include "utils/type_traits_macros.h"

namespace JumaEngine
{
    class VertexBufferImporterBase
    {
    public:
        VertexBufferImporterBase() = default;
        virtual ~VertexBufferImporterBase() = default;

        void importFile(const char* filePath);

        jarray<std::string> getMeshesNames() const;
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<VertexBufferDataBase, T>)>
        jarray<VertexBufferDataBase*> createVertexBufferForMesh(const std::string& meshName)
        {
            jarray<VertexBufferDataBase*> result;
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

        virtual VertexBufferImporterData loadDataFromFile(const char* filePath) const;

    private:

        VertexBufferImporterData m_Data;
    };
}
