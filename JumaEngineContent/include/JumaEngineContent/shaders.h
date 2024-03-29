// Copyright © 2023 Leonov Maksim. All rights reserved.

#pragma once

#include "core.h"

#include <jutils/jstring.h>

namespace JumaEngineContent
{
    void compileShaders(const jutils::jstring& contentType, bool forceRecompile = false);
}