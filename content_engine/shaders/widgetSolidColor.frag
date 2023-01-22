#version 450

layout(std140, binding = 1) uniform WidgetColorBlock
{
    vec4 uColor;
};

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = uColor;
}