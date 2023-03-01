// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../core.h"
#include "EngineContextObject.h"

namespace JumaEngine
{
    class EngineObjectOwner;

    class EngineObject : public EngineContextObject
    {
        JUMAENGINE_CLASS(EngineObject, EngineContextObject)

        friend EngineObjectOwner;

    public:
        EngineObject() = default;
        virtual ~EngineObject() override = default;

        bool isInitialized() const { return m_ObjectInitialized && !isDestroyed(); }
        bool isActive() const { return m_ObjectActivated; }
        bool isDestroyed() const { return m_ObjectDestroyed; }

    protected:

        virtual void onInitialized() {}
        virtual void onActivated() {}
        virtual void onUpdate(float deltaTime) {}
        virtual void onPreRender() {}
        virtual void onDeactivate() {}
        virtual void onClear() {}

        virtual void onObjectDescriptorDestroying() override { clearEngineObject(); }

    private:

        bool m_ObjectInitialized = false;
        bool m_ObjectActivated = false;
        bool m_ObjectDestroyed = false;


        void initializeEngineObject();
        void activateEngineObject();
        void update(float deltaTime);
        void preRender();
        void deactivateEngineObject();
        void clearEngineObject();
    };
}
