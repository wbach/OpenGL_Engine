#version 440

in vec2 position;
out vec2 textureCoords;

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

void main(void)
{
    gl_Position = perObjectUpdate.transformationMatrix * vec4(position, 0.0, 1.0);
    textureCoords = vec2((position.x+1.0) / 2.0, 1 - (-position.y + 1.0) / -2.0) ;
}