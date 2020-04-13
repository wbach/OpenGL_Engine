#version 440 core

in vec3 color;

out vec4 outputColor;

void main()
{
    outputColor = vec4(color, 1.f);

  // outputColor = vec4(0.f, 0.f, 0.f , 1.f);
}
