#version 440 core

layout (location = 0) in vec3 Position;

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

out VS_OUT
{
    vec3 worldPosition;
} vs_out;

void main()
{
	vec4 worldPosition 	   = perObjectUpdate.transformationMatrix * vec4(Position, 1.f);
    vs_out.worldPosition   = worldPosition.xyz;
    gl_Position 		   = worldPosition;
}
