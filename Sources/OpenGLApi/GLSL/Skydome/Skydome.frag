#version 430

const vec3 baseColor = vec3(0.18, 0.27, 0.47);

in vec4 worldPosition;

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 MaterialSpecular;

void main()
{
    float red = -0.00022f * (abs(worldPosition.y)-2800) + baseColor.x;
    float green = -0.00025f * (abs(worldPosition.y)-2800) + baseColor.y;
    float blue = -0.00019f * (abs(worldPosition.y)-2800) + baseColor.z;
    
    WorldPosOut = worldPosition;
    DiffuseOut  =  vec4(red, green, blue, 1.f);
    NormalOut        = vec4(0.f, 1.f, 0.f, 0.f); // w = 0; dont use fog on sky
    MaterialSpecular = vec4(0.f);
}