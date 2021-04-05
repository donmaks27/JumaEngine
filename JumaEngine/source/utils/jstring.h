// Copyright 2021 Leonov Maksim. All Rights Reserved.

#pragma once

#include "common_header.h"
#include <string>

namespace JumaEngine
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
    };
}
