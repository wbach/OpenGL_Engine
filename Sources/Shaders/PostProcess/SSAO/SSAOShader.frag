#version 400 core
in vec2 TexCoord;

out vec4 OutColor;

uniform sampler2D PositionTexture;

void main(void)
{    
	vec4 color = texture(PositionTexture, TexCoord);
    if ( color.a < 0.5f)
    {
        discard ;
    }    
    OutColor = color;	
}