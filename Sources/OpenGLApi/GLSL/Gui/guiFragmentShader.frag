#version 440

in vec2 textureCoords;

out vec4 outColor;

layout(binding = 0) uniform sampler2D guiTexture;

layout (std140, binding=6) uniform PerMeshObject
{
    vec4 color;
} perMeshObject;

void main(void)
{
    outColor = texture(guiTexture, textureCoords) * perMeshObject.color;
}