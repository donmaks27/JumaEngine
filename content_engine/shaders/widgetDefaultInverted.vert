#version 450

layout(std140, binding = 0) uniform WidgetLocationBlock
{
    vec2 uLocation;
    vec2 uSize;
    float uDepth;
};
layout(std140, binding = 1) uniform WidgetTextureTransformBlock
{
    vec2 uTextureOffset;
    vec2 uTextureScale;
};

layout(location = 0) in vec2 vPosition;
layout(location = 1) in vec2 vTexCoords;

layout(location = 0) out vec2 fTextureCoords;

void main()
{
    vec2 textureScaleAbs = abs(uTextureScale);
    vec2 textureCoordsMultiplier = uTextureScale / max(textureScaleAbs, 0.000001f);
    vec2 textureCoordsOffset = (vec2(1.0f) - textureCoordsMultiplier) / 2;
    fTextureCoords = vTexCoords * textureCoordsMultiplier + textureCoordsOffset;

    vec2 textureOffset = (1.0f - textureScaleAbs) / 2 * uSize;
    vec2 screenPosition = (uLocation + textureOffset + vPosition * textureScaleAbs * uSize) * 2 - 1.0f;
    gl_Position = vec4(screenPosition.x, -screenPosition.y, uDepth, 1.0f);
}