#version 420

layout(location = 0) in vec2 textureCoords;

#ifdef IS_VULKAN
    #define SAMPLER(set_, binding_) layout(set = set_, binding = binding_)
#else
    #define SAMPLER(set_, binding_) layout(binding = binding_)
#endif

SAMPLER(1, 0) uniform sampler2D modelTexture;

layout(location = 0) out vec4 FragColor;

void main()
{
// #ifdef IS_VULKAN
//     // tymczasowo , az nie bedize tekstur
//     FragColor = vec4(textureCoords, 0.0, 1.0);
// #else
    FragColor = texture(modelTexture, textureCoords);
//#endif
}
