#version 330

uniform sampler2D ColorMap;
in vec2 textureCoords;

out vec4 FragColor;

void main()
{
	vec2 tex_coord	= textureCoords;
	tex_coord.y*=-1;

	vec4 color = texture(ColorMap, tex_coord);
	FragColor.x = color.g;
	FragColor.y = color.b;
	FragColor.z = color.r;
	FragColor.w = color.a;
}
