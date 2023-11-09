// Copyright © 2023 Leonov Maksim. All rights reserved.

#include <JumaEngineContent/shaders.h>

int main(const int argCount, const char* args[])
{
    JumaEngineContent::compileShaders("engine");
    JumaEngineContent::compileShaders("game");
    return 0;
}
