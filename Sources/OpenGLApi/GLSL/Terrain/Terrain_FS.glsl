#version 430

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 SpecularOut;

uniform sampler2D normalmap;

in vec2 mapCoord_FS;
in vec4 worldPos;

void main()
{
    WorldPosOut = worldPos;
    DiffuseOut  = vec4(0.01,1,0.01,1.0);
    vec3 normal = normalize(texture(normalmap, mapCoord_FS).rgb);
    NormalOut   = vec4(normal, 1);
    SpecularOut = vec4(0);
}