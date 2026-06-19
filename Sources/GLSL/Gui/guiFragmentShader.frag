#version 440

layout(location = 0) in vec2 textureCoords;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D guiTexture;

layout (std140, binding=6) uniform ElementBuffer
{
    mat4 transformationMatrix;
    vec4 textureColor;
    vec4 backgroundColor;
    vec4 params; // x: 1.0 - has texture, 0.0 - no texture
} elementBuffer;

void main(void)
{
    vec4 sampledColor = vec4(0.0);
   
    if (elementBuffer.params.x > 0.5) 
    {
        sampledColor = texture(guiTexture, textureCoords) * elementBuffer.textureColor;
    }

    vec4 bgColor = elementBuffer.backgroundColor;

    float bgWeight = bgColor.a * (1.0 - sampledColor.a);
    vec3 finalRGB = (sampledColor.rgb * sampledColor.a) + (bgColor.rgb * bgWeight);

    float finalAlpha = clamp(sampledColor.a + bgColor.a, 0.0, 1.0);
    if (finalAlpha > 0.0) 
    {
        finalRGB /= finalAlpha;
    }

    outColor = vec4(finalRGB, finalAlpha);
}
