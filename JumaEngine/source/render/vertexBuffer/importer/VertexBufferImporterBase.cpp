// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "VertexBufferImporterBase.h"

namespace JumaEngine
{
    void VertexBufferImporterBase::importFile(const char* filePath)
    {
        m_Data = loadDataFromFile(filePath);
    }

    jarray<std::string> VertexBufferImporterBase::getMeshesNames() const
    {
        jarray<std::string> result;
        for (const auto& meshData : m_Data.meshesData)
        {
            result.push_back(meshData.name);
        }
        return result;
    }

    VertexBufferImporterData VertexBufferImporterBase::loadDataFromFile(const char* filePath) const
    {
        const jarray<VertexBufferImporterVertexData> vertices = {
            {{0.0f, -10.0f, -10.0f}},
            {{0.0f, 0.0f, -10.0f}},
            {{0.0f, 10.0f, 10.0f}}
        };
        return { { { JTEXT("Triangle"), Transform(), { { { vertices } } } } } };
    }
}
