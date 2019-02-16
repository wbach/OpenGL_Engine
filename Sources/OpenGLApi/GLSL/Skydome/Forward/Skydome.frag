#version 430

const vec3 baseColor = vec3(0.18, 0.27, 0.47);

in vec3 worldPosition;

layout(location = 0) out vec4 outputColor;

void main()
{
    float red = -0.00022f * (abs(worldPosition.y)-2800) + baseColor.x;
    float green = -0.00025f * (abs(worldPosition.y)-2800) + baseColor.y;
    float blue = -0.00019f * (abs(worldPosition.y)-2800) + baseColor.z;
    
    outputColor = vec4(red, green, blue, 1.f);
}