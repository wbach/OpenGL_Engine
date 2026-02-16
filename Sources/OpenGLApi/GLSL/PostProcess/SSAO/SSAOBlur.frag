#version 450 core

layout (binding = 0) uniform sampler2D ssaoInput;

in vec2 textureCoords;

out float FragColor;

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x) {
        for (int y = -2; y < 2; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, textureCoords + offset).r;
        }
    }
    result = result / 16.0;

    FragColor = result;
}
