#version 440 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Direction;

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

out VS_OUT
{
    vec3 worldPosition;
    vec3 worldDirection;
} vs_out;

void main()
{
    vec4 worldPosition      = perObjectUpdate.transformationMatrix * vec4(Position, 1.f);
    vec4 worldDirection     = perObjectUpdate.transformationMatrix * vec4(-Direction, 0.f);
    vs_out.worldPosition    = worldPosition.xyz;
    vs_out.worldDirection   = worldDirection.xyz;
    gl_Position             = worldPosition;
}
