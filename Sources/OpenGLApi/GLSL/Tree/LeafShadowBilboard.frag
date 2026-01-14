#version 450 core
in vec2 TexCoords;
flat in int TexIndex;

//uniform sampler2DArray shadowTextureArray;
layout(binding = 0) uniform sampler2D BaseColorTexture;

void main()
{
    //float alpha = texture(shadowTextureArray, vec3(TexCoords, float(TexIndex))).r;
    float alpha = texture(BaseColorTexture, TexCoords).r;
    if(alpha < 0.1)
    {
        discard;
    }
}