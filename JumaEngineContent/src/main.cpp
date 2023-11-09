// Copyright © 2023 Leonov Maksim. All rights reserved.

#include <filesystem>

#include <JumaEngineContent/shaders.h>

#include <jutils/log.h>
using namespace jutils;

jarray<jstring> getAvailableConfigsList();

int main(const int argCount, const char* args[])
{
    if (argCount > 1)
    {
        JumaEngineContent::compileShaders(args[1]);
    }
    else
    {
        for (const auto& config : getAvailableConfigsList())
        {
            JumaEngineContent::compileShaders(config);
        }
    }
    return 0;
}

jarray<jstring> getAvailableConfigsList()
{
    jarray<jstring> result;
    for (const auto& entry : std::filesystem::directory_iterator("./config/"))
    {
        if (!entry.is_regular_file() || (entry.path().extension() != ".ini"))
        {
            continue;
        }
        result.add(entry.path().stem().string());
    }
    return result;
}
