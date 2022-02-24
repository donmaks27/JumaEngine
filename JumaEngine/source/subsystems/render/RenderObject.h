// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

namespace JumaEngine
{
    template<typename ParentType>
    class RenderObject
    {
    public:
        RenderObject() = default;
        virtual ~RenderObject();

        bool init(ParentType* parent);
        void clear();

    protected:

        ParentType* m_Parent = nullptr;


        virtual bool initInternal() = 0;
        virtual void clearInternal() = 0;
    };

    template <typename ParentType>
    RenderObject<ParentType>::~RenderObject()
    {
        m_Parent = nullptr;
    }

    template <typename ParentType>
    bool RenderObject<ParentType>::init(ParentType* parent)
    {
        if (m_Parent != nullptr)
        {
            JUMA_LOG(error, JSTR("Render object already initialized"));
            return false;
        }
        if ((parent == nullptr) || !parent->isValid())
        {
            JUMA_LOG(error, JSTR("Parent object not valid"));
            return false;
        }
        
        m_Parent = parent;
        if (!initInternal())
        {
            JUMA_LOG(error, JSTR("Failed to initialized render object"));
            m_Parent = nullptr;
            return false;
        }
        return true;
    }

    template <typename ParentType>
    void RenderObject<ParentType>::clear()
    {
        if (m_Parent != nullptr)
        {
            clearInternal();
            m_Parent = nullptr;
        }
    }
}
