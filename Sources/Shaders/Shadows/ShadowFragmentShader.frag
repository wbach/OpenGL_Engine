#version 400 core
in vec2 textureCoords;

uniform sampler2D ModelTexture;

out vec4 out_colour;

void main(void)
{
    float alpha = texture(ModelTexture, textureCoords ).a;
    if ( alpha < 0.5f)
    {
        discard;
    }
    out_colour = vec4(1.0);
}