#version 330

uniform sampler2D ColorMap;
in vec2 textureCoords;

out vec4 FragColor;

float Offsets[4] = float[]( -1.5, -0.5, 0.5, 1.5 );

void main()
{
	vec2 tex_coord	= textureCoords;
	tex_coord.y*=-1;

    vec3 color = vec3(0.0, 0.0, 0.0);

    for (int i = 0 ; i < 4 ; i++) {
        for (int j = 0 ; j < 4 ; j++) {
            vec2 tc = tex_coord;
            tc.x = tex_coord.x + Offsets[j] / textureSize(ColorMap, 0).x;
            tc.y = tex_coord.y + Offsets[i] / textureSize(ColorMap, 0).y;
            color += texture(ColorMap, tc).xyz;
        }
    }

    color /= 16.0;

    FragColor = vec4(color, 1.0);
}
