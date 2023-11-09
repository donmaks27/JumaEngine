// Copyright © 2023 Leonov Maksim. All rights reserved.

#pragma once

#include <jutils/jstring.h>

namespace JumaEngineContent
{
    void compileShaders(const jutils::jstring& type, bool forceRecompile = false);
}