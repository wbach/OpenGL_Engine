#version 440 core

layout(location = 0) in vec3 color;

layout(location = 0) out vec4 outputColor;

void main()
{
    outputColor = vec4(color, 1.f);

  // outputColor = vec4(0.f, 0.f, 0.f , 1.f);
}
