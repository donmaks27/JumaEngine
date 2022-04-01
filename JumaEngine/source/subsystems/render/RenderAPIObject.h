// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

namespace JumaEngine
{
    template<typename ParentType>
    class RenderAPIObject
    {
        template<typename APIObjectType>
        friend class RenderAPIWrapperBase;

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
        bool init(RenderAPIWrapperBase<APIObjectType>* parent)
        {
            if (m_Parent != nullptr)
            {
                JUMA_LOG(error, JSTR("Render object already initialized"));
                return false;
            }
            if (parent == nullptr)
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
    class RenderAPIWrapperBase
    {
    public:

        using RenderAPIObjectType = APIObjectType;

        RenderAPIWrapperBase() = default;
        virtual ~RenderAPIWrapperBase() = default;

        bool createRenderAPIObject()
        {
            if (m_RenderAPIObject != nullptr)
            {
                JUMA_LOG(warning, JSTR("Render API object already created"));
                return false;
            }

            m_RenderAPIObject = createRenderAPIObjectInternal();
            if (m_RenderAPIObject == nullptr)
            {
                JUMA_LOG(error, JSTR("Failed to create render API object"));
                return false;
            }

            if (!m_RenderAPIObject->init(this))
            {
                JUMA_LOG(error, JSTR("Failed to initialize render API object"));
                delete m_RenderAPIObject;
                m_RenderAPIObject = nullptr;
                return false;
            }
            return true;
        }
        APIObjectType* getRenderAPIObject() const { return m_RenderAPIObject; }
        template<typename Type, TEMPLATE_ENABLE(is_base_and_not_same<APIObjectType, Type>)>
        Type* getRenderAPIObject() const { return dynamic_cast<Type*>(m_RenderAPIObject); }
        void clearRenderAPIObject()
        {
            if (m_RenderAPIObject != nullptr)
            {
                delete m_RenderAPIObject;
                m_RenderAPIObject = nullptr;
            }
        }

    protected:

        virtual APIObjectType* createRenderAPIObjectInternal() = 0;

    private:

        APIObjectType* m_RenderAPIObject = nullptr;
    };
    template<typename APIObjectType>
    class RenderAPIWrapper : public RenderAPIWrapperBase<APIObjectType>
    {
    public:

        RenderAPIWrapper() = default;
        virtual ~RenderAPIWrapper() override = default;

        bool isValid() const { return m_Initialized; }
        void clear()
        {
            if (isValid())
            {
                clearInternal();
                m_Initialized = false;
            }
        }

    protected:

        void markAsInitialized() { m_Initialized = true; }

        virtual void clearInternal() = 0;

    private:

        bool m_Initialized = false;
    };
}
