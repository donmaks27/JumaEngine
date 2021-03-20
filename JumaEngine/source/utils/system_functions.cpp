// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "system_functions.h"
#include "Engine.h"
#include "EngineContextObject.h"
#include "render/RenderManagerBase.h"
#include "render/mesh/vertex/VertexBufferBase.h"

namespace JumaEngine
{
    VertexBufferBase* SystemFunctions::createVertexBuffer(const EngineContextObject* engineContextObject, VertexBufferDataBase* vertexBufferData)
    {
        if (engineContextObject != nullptr)
        {
            const Engine* engine = engineContextObject->getOwnerEngine();
            RenderManagerBase* renderManager = engine != nullptr ? engine->getRenderManager() : nullptr;

            VertexBufferBase* vertexBuffer = renderManager->createVertextBufferRender();
            if (vertexBufferData != nullptr)
            {
                vertexBuffer->init(vertexBufferData);
            }
            return vertexBuffer;
        }
        return nullptr;
    }
}
