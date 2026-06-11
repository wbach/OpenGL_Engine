#version 450 core
layout(location = 0) in vec2 TexCoords;
layout(location = 1) flat in int TexIndex;

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