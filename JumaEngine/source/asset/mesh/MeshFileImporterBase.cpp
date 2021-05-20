// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "MeshFileImporterBase.h"

namespace JumaEngine
{
    void MeshFileImporterBase::importFile(const jstring& filePath, const bool forceLoad)
    {
        if (forceLoad || (filePath != m_LoadedMeshFilePath))
        {
            clear();
            m_LoadedMeshFilePath = filePath;
            if (!m_LoadedMeshFilePath.empty())
            {
                importMeshFileInternal();
            }
        }
    }

    void MeshFileImporterBase::clear()
    {
        m_LoadedMeshFilePath.clear();
        m_Data = MeshFileImporterData();
    }
}
