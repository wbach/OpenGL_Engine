#version 440

const float tiling = 10.0;
const float viewDistance = 450.0 ;
const float transitionDistance = 10.0;
const float density = 0.0035 ;
const float gradient = 2.5 ;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;

layout (std140,binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
} perFrame;

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 transformationMatrix;
} perObjectUpdate;

out VS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec3 worldPos;
    vec3 cameraPosition;
    vec4 clipSpace;
} vs_out;

void main(void)
 {
    vec4 worldSpace       = perObjectUpdate.transformationMatrix * vec4(Position, 1.0);
    vs_out.texCoord       = TexCoord * tiling;
    vs_out.normal         = (perObjectUpdate.transformationMatrix * vec4(Normal, 0.0)).xyz;
    vs_out.worldPos       = worldSpace.xyz;
    vs_out.cameraPosition = perFrame.cameraPosition;
    vs_out.clipSpace      = perFrame.projectionViewMatrix * worldSpace;

    gl_Position = vs_out.clipSpace;
}