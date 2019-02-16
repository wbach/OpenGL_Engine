#version 420

layout (location = 0) in vec3 Position;

out vec3 TextureCoords;

layout (std140, binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    mat4 toShadowMapSpace;
    vec3 cameraPosition;
} perFrame;

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

void main(void)
{
    gl_Position = perFrame.projectionViewMatrix * perObjectUpdate.transformationMatrix * vec4(Position, 1.f);
    TextureCoords = Position;
}