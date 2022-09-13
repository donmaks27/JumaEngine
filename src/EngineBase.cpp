// Copyright 2022 Leonov Maksim. All Rights Reserved.

#include "../include/JumaEngine/EngineBase.h"
#include "../include/JumaEngine/EngineContextObject.h"

namespace JumaEngine
{
    bool EngineBase::init()
    {
        if (!initInternal())
        {
            JUTILS_LOG(error, JSTR("Failed to initialize Engine"));
            return false;
        }
        JUTILS_LOG(correct, JSTR("Engine initialied"));
        return true;
    }
    void EngineBase::destroy()
    {
        destroyInternal();
        JUTILS_LOG(info, JSTR("Engine destroyed"));
    }

    void EngineBase::registerObjectInternal(EngineContextObject* object)
    {
        if (object != nullptr)
        {
            object->m_Engine = this;
        }
    }
}
