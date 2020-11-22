#version 420
const int MAX_INSTANCES = 100;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;

layout (std140, binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
} perFrame;

layout (std140, binding=3) uniform PerObjectUpdate
{
    mat4 normalizationMatrix;
} perObjectUpdate;

layout (std140, binding=5) uniform PerInstances
{
    mat4 transformMatrixes[MAX_INSTANCES];
} perInstances;

out VS_OUT
{
    vec4 position;
    vec2 textureCoords;
    vec3 normal;
} vs_out;

void main(void)
{
    vs_out.textureCoords = TexCoord;
    mat4 transform = perInstances.transformMatrixes[gl_InstanceID] * perObjectUpdate.normalizationMatrix;
    vs_out.normal = (transform * vec4(Normal, 0.f)).xyz;
    gl_Position =  perFrame.projectionViewMatrix * transform  * vec4(Position, 1.0);
}