// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

namespace JumaEngine
{
    template<typename ParentType>
    class RenderAPIObject
    {
        template<typename APIObjectType>
        friend class RenderObject;

    public:
        RenderAPIObject() = default;
        virtual ~RenderAPIObject()
        {
            m_Parent = nullptr;
        }

    protected:

        ParentType* m_Parent = nullptr;


        virtual bool initInternal() = 0;

    private:

        template<typename APIObjectType>
        bool init(RenderObject<APIObjectType>* parent)
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
            
            m_Parent = dynamic_cast<ParentType*>(parent);
            if ((m_Parent == nullptr) || !initInternal())
            {
                JUMA_LOG(error, JSTR("Failed to initialized render object"));
                m_Parent = nullptr;
                return false;
            }
            return true;
        }
    };

    template<typename APIObjectType>
    class RenderObject
    {
    public:

        using RenderAPIObjectType = APIObjectType;

        RenderObject() = default;
        virtual ~RenderObject() = default;

        bool isValid() const { return m_Initialized; }
        void clear()
        {
            if (isValid())
            {
                clearInternal();
                m_Initialized = false;
            }
        }

        bool createRenderAPIObject()
        {
            if (!isValid())
            {
                JUMA_LOG(warning, JSTR("Shader not initialized"));
                return false;
            }

            m_RenderAPIObject = createRenderAPIObjectInternal();
            if (!m_RenderAPIObject->init(this))
            {
                delete m_RenderAPIObject;
                m_RenderAPIObject = nullptr;
                return false;
            }
            return true;
        }
        APIObjectType* getRenderAPIObject() const { return m_RenderAPIObject; }
        void clearRenderAPIObject()
        {
            if (m_RenderAPIObject != nullptr)
            {
                delete m_RenderAPIObject;
                m_RenderAPIObject = nullptr;
            }
        }

    protected:

        void markAsInitialized() { m_Initialized = true; }

        virtual APIObjectType* createRenderAPIObjectInternal() = 0;

        virtual void clearInternal() = 0;

    private:

        bool m_Initialized = false;
        APIObjectType* m_RenderAPIObject = nullptr;
    };
}
