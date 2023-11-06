cmake_minimum_required(VERSION 3.12)

project(JumaEngineContent)

list(APPEND JUMAEC_SOURCE_FILES
    src/main.cpp
)

add_executable(JumaEngineContent ${JUMAEC_SOURCE_FILES})
target_link_libraries(JumaEngineContent PRIVATE JumaShaderCompiler)