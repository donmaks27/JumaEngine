#version 450

layout(std140, binding = 0) uniform WidgetLocationBlock
{
    vec2 uLocation;
    vec2 uSize;
    float uDepth;
};

layout(location = 0) in vec2 vPosition;
layout(location = 1) in vec2 vTexCoords;

void main()
{
    gl_Position = vec4(uLocation + uSize * vPosition, uDepth, 1.0f);
}