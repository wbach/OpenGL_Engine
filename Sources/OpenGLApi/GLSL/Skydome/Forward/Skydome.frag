#version 430 core

const vec3 baseColor = vec3(0.18, 0.27, 0.47);

in vec4 worldPosition;

layout(location = 0) out vec4 outputColor;

void main()
{
    float factor = 2.f;
    float yOffset = 2400.f * factor;
    float red = -0.00022f / factor * (abs(worldPosition.y) - yOffset) + baseColor.x;
    float green = -0.00025f / factor * (abs(worldPosition.y) - yOffset) + baseColor.y;
    float blue = -0.00019f / factor * (abs(worldPosition.y) - yOffset) + baseColor.z;

    outputColor = vec4(red, green, blue, 1.f);
}