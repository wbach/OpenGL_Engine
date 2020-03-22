#version 440

in vec2 textureCoords;

out vec4 out_Color;

layout(binding = 0) uniform sampler2D guiTexture;

layout (std140, binding=6) uniform PerMeshObject
{
    vec3 color;
} perMeshObject;

void main(void)
{
    out_Color = texture(guiTexture, textureCoords) * vec4(perMeshObject.color, 1.f);
}