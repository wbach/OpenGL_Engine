#version 440

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 Weights;
layout (location = 5) in ivec3 BoneIds;

layout (std140,binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    mat4 toShadowMapSpace;
    vec3 cameraPosition;
} perFrame;

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

out VS_OUT
{
    vec4 worldPos;
    vec2 texCoord;
    vec4 normal;
} vs_out;

void main()
{
    vs_out.worldPos = perFrame.projectionViewMatrix * perObjectUpdate.transformationMatrix * vec4(Position, 1.f);
    vs_out.normal   = perFrame.projectionViewMatrix * perObjectUpdate.transformationMatrix * vec4(Normal, 1.0);
    vs_out.texCoord = TexCoord;
    gl_Position     = vs_out.worldPos;
}