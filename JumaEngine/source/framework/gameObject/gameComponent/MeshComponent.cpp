// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "MeshComponent.h"
#include "CameraComponent.h"
#include "asset/material/MaterialBase.h"
#include "asset/mesh/Mesh.h"
#include "utils/system_functions.h"

namespace JumaEngine
{
    void MeshComponent::setMesh(const asset_ptr<Mesh>& mesh)
    {
        m_Mesh = mesh;
    }

    void MeshComponent::render(const window_id windowID, const RenderParams& renderParams)
    {
        if (m_Mesh != nullptr)
        {
    		MaterialBase* material = m_Mesh->getMaterial(0).get();
    		if (material != nullptr)
    		{
    			const CameraComponent* camera = SystemFunctions::getWindowActiveCamera(this, windowID);
				if (camera != nullptr)
				{
					material->setMaterialParam("uProjection", camera->getProjectionMatrix());
					material->setMaterialParam("uView", camera->getViewMatrix());
				}
				material->setMaterialParam("uModel", getWorldTransform().toMatrix());
			}

            const glm::vec3 scale = getWorldScale();
            uint8 negativeCount = 0;
            if (scale.x < 0)
            {
                ++negativeCount;
            }
            if (scale.y < 0)
            {
                ++negativeCount;
            }
            if (scale.z < 0)
            {
                ++negativeCount;
            }
            const bool shouldInvertFacesOrientation = negativeCount % 2 == 0 ? renderParams.invertFacesOrientation : !renderParams.invertFacesOrientation;

            RenderParams newRenderParams = renderParams;
            newRenderParams.invertFacesOrientation = negativeCount % 2 == 0 ? renderParams.invertFacesOrientation : !renderParams.invertFacesOrientation;
            m_Mesh->render(windowID, newRenderParams);
        }

        Super::render(windowID, renderParams);
    }
}
