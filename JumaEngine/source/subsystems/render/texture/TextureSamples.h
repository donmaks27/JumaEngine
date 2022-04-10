// Copyright 2022 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"

namespace JumaEngine
{
    enum class TextureSamples : uint8
    {
        SAMPLES_1,
        SAMPLES_2,
        SAMPLES_4,
        SAMPLES_8,
        SAMPLES_16,
        SAMPLES_32,
        SAMPLES_64
    };

    inline uint8 GetTextureSamplesNumber(const TextureSamples samples)
    {
        switch (samples)
        {
        case TextureSamples::SAMPLES_1:  return 1;
        case TextureSamples::SAMPLES_2:  return 2;
        case TextureSamples::SAMPLES_4:  return 4;
        case TextureSamples::SAMPLES_8:  return 8;
        case TextureSamples::SAMPLES_16: return 16;
        case TextureSamples::SAMPLES_32: return 32;
        case TextureSamples::SAMPLES_64: return 64;
        default: ;
        }
        JUMA_LOG(error, JSTR("Unsupported sample count"));
        return 0;
    }
}
