// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

namespace JumaEngine
{
    class Engine;

    class EngineContextObject
    {
        friend Engine; 

    protected:
        EngineContextObject() = default;
        virtual ~EngineContextObject() = default;

    public:

        Engine* getOwnerEngine() const { return m_OwnerEngine; }

    protected:

    	virtual void onRegister() {}

    private:

        Engine* m_OwnerEngine = nullptr;
    };
}
