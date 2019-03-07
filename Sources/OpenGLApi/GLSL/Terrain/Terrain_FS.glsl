#version 330

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 SpecularOut;

in vec4 worldPos;

void main()
{
    WorldPosOut = worldPos;
    DiffuseOut  = vec4(0.01,1,0.01,1.0);
    NormalOut   = vec4(0,1,0,0);
    SpecularOut = vec4(0);
}