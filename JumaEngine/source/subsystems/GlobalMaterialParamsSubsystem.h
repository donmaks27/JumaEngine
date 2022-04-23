// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include "subsystems/SubsystemBase.h"

#include "render/material/MaterialParamsStorage.h"

namespace JumaEngine
{
    class GlobalMaterialParamsSubsystem : public SubsystemBase
    {
        JUMAENGINE_CLASS(GlobalMaterialParamsSubsystem, SubsystemBase)

    public:
        GlobalMaterialParamsSubsystem() = default;
        virtual ~GlobalMaterialParamsSubsystem() override = default;

        MaterialParamsStorage& getGlobalMaterialParams() { return m_GlobalMaterialParams; }

    protected:

        void clearSubsystemInternal() override { m_GlobalMaterialParams.clear(); }

    private:

        MaterialParamsStorage m_GlobalMaterialParams;
    };
}
