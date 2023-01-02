// Copyright © 2023 Leonov Maksim. All Rights Reserved.

#pragma once

#include "../../core.h"
#include "../EngineSubsystem.h"

#include <JumaRE/vertex/VertexBuffer.h>

namespace JumaEngine
{
    class UIObject;

    class UISubsystem final : public EngineSubsystem
    {
        JUMAENGINE_CLASS(UISubsystem, EngineSubsystem)

    public:
        UISubsystem() = default;
        virtual ~UISubsystem() override = default;

        JumaRE::VertexBuffer* getVertexBufferUI() const { return m_VertexBufferUI; }

        UIObject* createUIObject();

    protected:

        virtual bool initSubsystem() override;
        virtual void clearSubsystem() override;

    private:

        JumaRE::VertexBuffer* m_VertexBufferUI = nullptr;

        jarray<UIObject*> m_UIObjects;
    };
}
