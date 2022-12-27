cmake_minimum_required(VERSION 3.12)

project(JumaEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND JUMA_ENGINE_LIBS JumaRE)

add_library(JumaEngine STATIC 
    src/Engine.cpp
    src/GameEngine.cpp
    src/GameInstance.cpp
    src/subsystems/meshes/Mesh.cpp
    src/subsystems/meshes/MeshesSubsystem.cpp
    src/subsystems/shaders/Shader.cpp
    src/subsystems/shaders/Material.cpp
    src/subsystems/shaders/ShadersSubsystem.cpp
    src/subsystems/textures/Texture.cpp
    src/subsystems/textures/TexturesSubsystem.cpp
)
target_include_directories(JumaEngine INTERFACE $<TARGET_PROPERTY:JumaRE,INTERFACE_INCLUDE_DIRECTORIES> include)
target_link_libraries(JumaEngine PRIVATE JumaRE)

install(DIRECTORY include/JumaEngine/ DESTINATION include/JumaEngine FILES_MATCHING PATTERN "*.h")
install(TARGETS JumaEngine DESTINATION lib)