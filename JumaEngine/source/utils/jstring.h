// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include <string>

namespace JumaEngine
{
    class jstring : public std::string
    {
    public:
        jstring()
            : std::string()
        {}
        jstring(const char* str)
            : std::string(str)
        {}
        jstring(const std::string& str)
            : std::string(str)
        {}
    };
}
