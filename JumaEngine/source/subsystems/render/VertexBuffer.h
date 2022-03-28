// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "RenderObject.h"
#include "engine/EngineContextObject.h"

#include "jutils/jstringID.h"

namespace JumaEngine
{
    struct RenderOptions;
    class VertexBuffer;
    class VertexBufferDataBase;

    class VertexBufferObject : public RenderObject<VertexBuffer>
    {
    public:
        VertexBufferObject() = default;
        virtual ~VertexBufferObject() override = default;

        virtual bool render(const RenderOptions* renderOptions) = 0;

    protected:

        inline const VertexBufferDataBase* getVertexData() const;
    };

    class VertexBuffer final : public EngineContextObject
    {
        JUMAENGINE_CLASS(VertexBuffer, EngineContextObject)

        friend VertexBufferObject;

    public:
        VertexBuffer() = default;
        virtual ~VertexBuffer() override;

        bool init(const VertexBufferDataBase* data);
        bool isValid() const { return m_Initialized; }
        void clear();

        const jstringID& getVertexName() const { return m_VertexName; }
        uint32 getVertexCount() const { return m_VertexCount; }
        uint32 getIndexCount() const { return m_IndexCount; }

        bool createRenderObject();
        VertexBufferObject* getRenderObject() const { return m_RenderObject; }
        void clearRenderObject();

        bool render(const RenderOptions* renderOptions) const;

    private:

        bool m_Initialized = false;

        const VertexBufferDataBase* m_VertexData = nullptr;
        jstringID m_VertexName = jstringID_NONE;
        uint32 m_VertexCount = 0;
        uint32 m_IndexCount = 0;

        VertexBufferObject* m_RenderObject = nullptr;
    };
    
    const VertexBufferDataBase* VertexBufferObject::getVertexData() const
    {
        return m_Parent->m_VertexData;
    }
}
