// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "subsystems/SubsystemBase.h"
#include "subsystems/render/RenderInterface.h"
#include "subsystems/render/window/WindowDescriptionBase.h"

namespace JumaEngine
{
    class RenderSubsystem : public SubsystemBase, public IRenderInterface
    {
        JUMAENGINE_CLASS(RenderSubsystem, SubsystemBase)

    public:
        RenderSubsystem() = default;
        virtual ~RenderSubsystem() override = default;

    protected:

        virtual bool initSubsystem() override;
        virtual void terminateSubsystem() override;

        virtual WindowDescriptionBase* createWindowInternal(const glm::uvec2& size, const jstring& title) = 0;

    private:

        WindowDescriptionBase* m_MainWindow = nullptr;
    };
}
