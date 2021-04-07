// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "MeshComponent.h"
#include "CameraComponent.h"
#include "framework/material/MaterialBase.h"
#include "framework/mesh/Mesh.h"
#include "utils/system_functions.h"

namespace JumaEngine
{
    void MeshComponent::setMesh(Mesh* mesh)
    {
        m_Mesh = mesh;
    }

    void MeshComponent::render()
    {
        if (m_Mesh != nullptr)
        {
            m_Mesh->render();
        }

        Super::render();
    }

    void MeshComponent::postTick()
    {
    	Super::postTick();

    	if (m_Mesh != nullptr)
    	{
    		MaterialBase* material = m_Mesh->getMaterial(0);
    		if (material != nullptr)
    		{
    			const CameraComponent* camera = SystemFunctions::getActiveCamera(this);
				if (camera != nullptr)
				{
					material->setMaterialParam("uProjection", camera->getProjectionMatrix());
					material->setMaterialParam("uView", camera->getViewMatrix());
				}
				material->setMaterialParam("uModel", getWorldTransform().toMatrix());
    		}
    	}
    }
}
