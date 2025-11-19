#version 440 core

//Attributes
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 Tangent;

layout (std140,binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
    vec4 projection;
} perFrame;

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

out VS_OUT
{
    vec4 worldPosition;
    vec4 normal;
    vec4 tangent;
} vs_out;

void main()
{
    mat4 normalTransformationMatrix = transpose(inverse(perObjectUpdate.transformationMatrix));
    vs_out.worldPosition = perObjectUpdate.transformationMatrix * vec4(Position, 1.f);
    vec4 normal = normalTransformationMatrix * vec4(Normal, 1.f);
    vec4 tangent = normalTransformationMatrix * vec4(Tangent, 1.f);

    vs_out.normal  = perFrame.projectionViewMatrix * (vs_out.worldPosition + vec4(normalize(normal.xyz), 0.f));
    vs_out.tangent = perFrame.projectionViewMatrix * (vs_out.worldPosition + vec4(normalize(tangent.xyz), 0.f));
    gl_Position    = perFrame.projectionViewMatrix * vs_out.worldPosition;
}