#version 440 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 SizeRandomnessAndTextureIndex;
layout (location = 2) in vec3 Direction;
layout (location = 3) in vec3 ColorRandomness;

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

out VS_OUT
{
    vec3 worldPosition;
    vec3 worldDirection;
    vec3 colorRandomness;
    float sizeRanfomness;
    flat int textureIndex;
} vs_out;

void main()
{
    vec4 worldPosition      = perObjectUpdate.transformationMatrix * vec4(Position, 1.f);
    vec4 worldDirection     = perObjectUpdate.transformationMatrix * vec4(-Direction, 0.f);
    vs_out.worldPosition    = worldPosition.xyz;
    vs_out.worldDirection   = worldDirection.xyz;
    vs_out.sizeRanfomness   = SizeRandomnessAndTextureIndex.x;
    vs_out.textureIndex     = int(SizeRandomnessAndTextureIndex.y);
    vs_out.colorRandomness  = ColorRandomness;
    gl_Position             = worldPosition;
}
