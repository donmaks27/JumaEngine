// Copyright 2021 Leonov Maksim. All Rights Reserved.

#include "Engine.h"
#include "window/WindowGLFW.h"

int main(int argc, char** argv)
{
    JumaEngine::Engine engine;
    JumaEngine::WindowGLFW window;
    return engine.startEngine(argc, argv, &window);
}
