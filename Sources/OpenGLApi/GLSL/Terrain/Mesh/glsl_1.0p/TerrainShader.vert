#version 130

in vec3 POSITION;
in vec2 TEXTCOORD;
in vec3 NORMAL;
in vec3 TANGENT;

uniform mat4 transformMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec2 texCoord;
out vec3 normal;

void main()
{
    vec4 worldPos           = transformMatrix* vec4(POSITION, 1.0);
    vec4 modelViewPosition  = viewMatrix * worldPos;
    texCoord         = TEXTCOORD;
    normal           = (transformMatrix * vec4(NORMAL, 0.0)).xyz;
    gl_Position = projectionMatrix * modelViewPosition;
}