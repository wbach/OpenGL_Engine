#version 440 core

in vec2 textureCoords;
out vec4 outputColor;

void main()
{
    float gridScale = 0.01f;
    float lineScale = 0.05f;

    if(fract(textureCoords.x / gridScale) < lineScale || fract(textureCoords.y / gridScale) < lineScale)
    {
        outputColor = vec4(0.05f, 0.05f, 0.05f, 0.25f);
    }
    else
    {
        discard;
    }
}