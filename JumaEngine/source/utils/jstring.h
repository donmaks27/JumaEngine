// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include <string>

namespace jutils
{
    class jstring : public std::string
    {
    public:
        using base_class = std::string;

        jstring()
            : base_class()
        {}
        jstring(const char* str)
            : base_class(str)
        {}
        jstring(const base_class& str)
            : base_class(str)
        {}

        const char* operator*() const noexcept { return this->c_str(); }
    };
}

#define JSTR(str) u8 ## str
#define TO_JSTR(value) std::to_string(value)
