#version 420

layout(location = 0) in vec2 TexCoordOut;

layout(binding = 0) uniform sampler2D modelTexture;

layout(location = 0) out vec4 FragColor;

void main()
{
    vec4 textureColor = texture(modelTexture, TexCoordOut);
    FragColor = textureColor;
}