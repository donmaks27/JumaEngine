cmake_minimum_required(VERSION 3.12)

project(JumaEngineContent)

list(APPEND JUMAEC_HEADER_FILES
    include/JumaEngineContent/shaders.h
)
list(APPEND JUMAEC_SOURCE_FILES
    src/shaders.cpp
)

add_library(JumaEngineContent STATIC ${JUMAEC_HEADER_FILES} ${JUMAEC_SOURCE_FILES})
target_include_directories(JumaEngineContent PUBLIC include)
target_link_libraries(JumaEngineContent PUBLIC jutils PRIVATE JumaShaderCompiler)

add_executable(JumaEC_CompileShaders src/main.cpp)
target_link_libraries(JumaEC_CompileShaders PRIVATE JumaEngineContent)
set_target_properties(JumaEC_CompileShaders PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")