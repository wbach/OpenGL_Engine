#version 330 core
in vec2 textureCoords ;

out vec4 out_colour;

uniform sampler2D modelTexture;//will use this next week

void main(void)
{    
	vec4 color = texture(modelTexture,textureCoords);
    if ( color.a < 0.5f)
    {
        discard ;
    }    
    out_colour = color;	
}