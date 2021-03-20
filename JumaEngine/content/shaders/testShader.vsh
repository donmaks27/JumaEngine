#version 460

layout(location = 0) uniform mat4 uProjection;
layout(location = 1) uniform mat4 uView;
layout(location = 3) uniform mat4 uModel;

layout(location = 0) in vec3 vPosition;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(vPosition, 1);
}