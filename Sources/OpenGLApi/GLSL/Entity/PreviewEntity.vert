#version 440
const int MAX_BONES = 512;
const int MAX_WEIGHTS = 4;

//Attributes
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec4 Weights;
layout (location = 5) in ivec4 BoneIds;

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    float viewDistance;
    vec3 shadowVariables;
} perApp;

layout (std140,binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
} perFrame;

out VS_OUT
{
    vec2 texCoord;
    vec3 normal;
} vs_out;

void main()
{
    vs_out.texCoord = TexCoord;
    vs_out.normal   = (perFrame.projectionViewMatrix *  vec4(Normal, 1.f)).xyz;
    gl_Position     = perFrame.projectionViewMatrix *  vec4(Position, 1.f);
}