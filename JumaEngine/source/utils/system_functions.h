// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "Engine.h"
#include "render/window/window_id.h"

namespace JumaEngine
{
    class MeshImporterBase;
    class CameraComponent;

    class SystemFunctions final
    {
    public:

		static Engine* getEngine(const EngineContextObject* engineContextObject) { return engineContextObject != nullptr ? engineContextObject->getOwnerEngine() : nullptr; }
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_same<EngineContextObject, T>)>
        static T* createObject(const EngineContextObject* engineContextObject)
        {
            Engine* engine = getEngine(engineContextObject);
            if (engine != nullptr)
            {
                return engine->createObject<T>();
            }
            return nullptr;
        }
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_same<EngineContextObject, T>)>
        static T* createObject(const EngineContextObject* engineContextObject, const subclass<T>& objectClass)
        {
            Engine* engine = getEngine(engineContextObject);
            if (engine != nullptr)
            {
                return engine->createObject<T>(objectClass);
            }
            return nullptr;
        }

        static MeshImporterBase* getVertexBufferImporter(const EngineContextObject* engineContextObject)
		{
		    const Engine* engine = getEngine(engineContextObject);
            return engine != nullptr ? engine->getVertexBufferImporter() : nullptr;
		}

        static CameraComponent* getWindowActiveCamera(const EngineContextObject* engineContextObject, window_id windowID);
    };
}
