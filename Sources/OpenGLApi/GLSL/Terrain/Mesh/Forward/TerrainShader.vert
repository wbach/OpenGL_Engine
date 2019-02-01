#version 330

layout (location = 0) in vec3 POSITION;
layout (location = 1) in vec2 TEXTCOORD;
layout (location = 2) in vec3 NORMAL;
layout (location = 3) in vec3 TANGENT;

uniform mat4 transformMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out VS_OUT
{
    vec2 texCoord;
    vec3 normal;
} vs_out;

void main()
{
    vec4 worldPos           = transformMatrix* vec4(POSITION, 1.0);
    vec4 modelViewPosition  = viewMatrix * worldPos;
    vs_out.texCoord         = TEXTCOORD;
    vs_out.normal           = (transformMatrix * vec4(NORMAL, 0.0)).xyz;
    gl_Position = projectionMatrix * modelViewPosition;
}