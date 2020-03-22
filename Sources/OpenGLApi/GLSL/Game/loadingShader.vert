#version 440

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 textureCoords;

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

out vec2 TexCoordOut;

void main()
{
    gl_Position = perObjectUpdate.transformationMatrix * vec4(position, 1.0);
    TexCoordOut = textureCoords;
}