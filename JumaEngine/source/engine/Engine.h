// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "EngineContextObject.h"
#include "subsystems/render/RenderInterface.h"
#include "jutils/jshared_ptr.h"

namespace JumaEngine
{
    class RenderPrimitive;
    class RenderSubsystem;

    class Engine final : public IRenderInterface
    {
    public:
        Engine() = default;
        virtual ~Engine() override = default;

        EngineContextObject* createObject(const EngineContextObject::ClassType* objectClass);
        template<typename T, TEMPLATE_ENABLE(is_base_and_not_abstract<EngineContextObject, T>)>
        T* createObject() { return EngineContextObject::cast<T>(createObject(T::getClass())); }

        bool startEngine();
        
        virtual void render(const RenderOptions& options) override;

        RenderSubsystem* getRenderSubsystem() const { return m_RenderSubsystem; }

    private:

        bool m_Started = false;

        RenderSubsystem* m_RenderSubsystem = nullptr;

        jshared_ptr<RenderPrimitive> m_RenderPrimitive = nullptr;


        void registerEngineObject(EngineContextObject* object);

        bool startEngineInternal();
        bool initEngine();

        void startEngineLoop();

        void terminate();
    };
}
