#version 420

in vec2 textureCoords;

out vec4 out_Color;

uniform sampler2D guiTexture;

layout (std140, binding=6) uniform PerMeshObject
{
    vec3 color;
} perMeshObject;

void main(void)
{
    vec4 tcolor = texture(guiTexture, textureCoords);
    out_Color = vec4(perMeshObject.color, 1.0) * tcolor;
}