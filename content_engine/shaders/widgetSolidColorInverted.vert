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
    vec2 screenPosition = (uLocation + uSize * vPosition) * 2 - 1.0f;
    gl_Position = vec4(screenPosition.x, -screenPosition.y, uDepth, 1.0f);
}