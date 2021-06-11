#version 440 core

layout (std140, align=16, binding=6) uniform InputBuffer
{
    vec2 textureSize;
} inputBuffer;

uniform sampler2D ColorMap;
in vec2 textureCoords;

out vec4 FragColor;

void main()
{
    mat3 sobelx = mat3
    (
         1.0,  2.0,  1.0,
         0.0,  0.0,  0.0,
        -1.0, -2.0, -1.0
    );
    mat3 sobely = mat3
    (
         1.0,  0.0,  -1.0,
         2.0,  0.0,  -2.0,
         1.0,  0.0,  -1.0
    );

    mat3 magnitudes = mat3(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            vec2 coords = vec2(textureCoords.x + (float(i) - 1.0) / inputBuffer.textureSize.x, textureCoords.y + (float(j) - 1.0) / inputBuffer.textureSize.y);
            magnitudes[i][j] = length(texture(ColorMap, coords).rgb);
        }
    }

    float x = dot(sobelx[0], magnitudes[0]) + dot(sobelx[1], magnitudes[1]) + dot(sobelx[2], magnitudes[2]);
    float y = dot(sobely[0], magnitudes[0]) + dot(sobely[1], magnitudes[1]) + dot(sobely[2], magnitudes[2]);

    float final = pow(sqrt(x * x + y * y) / 2.0, 0.5);

    FragColor = vec4(final, final, final, 1.0);
}
