#version 420

in vec2 TexCoordOut;

uniform sampler2D modelTexture;

out vec4 FragColor;

void main()
{
	vec4 textureColor = texture(modelTexture, TexCoordOut);
    FragColor = textureColor;
}