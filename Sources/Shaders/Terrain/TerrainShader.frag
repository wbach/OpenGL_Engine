#version 330

in TES_OUT
{
    vec2 tc;
    vec3 world_coord;
    vec3 eye_coord;
    vec4 position;
} fs_in;

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 SpecularOut;

uniform sampler2D BlendMap;
uniform sampler2D BackgroundTexture;
uniform sampler2D rTexture;
uniform sampler2D gTexture;
uniform sampler2D bTexture;
uniform sampler2D BackgroundTextureNormal;
uniform sampler2D rTextureNormal;
uniform sampler2D gTextureNormal;
uniform sampler2D bTextureNormal;

uniform sampler2D RockTexture;
uniform sampler2D RockTextureNormal;

uniform sampler2D shadowMap;

vec4 CalculateTerrainColor()
{
	vec4 blend_map_colour = texture(BlendMap, fs_in.tc) ;
		
	float back_texture_amount = 1 - (blend_map_colour.r + blend_map_colour.g + blend_map_colour.b) ;
	vec2 tiled_coords = fs_in.tc * 40.0f ;

	float normal_y = 1.f;//abs(normalize(In.normal).y);

	normal_y = normal_y *2;

	if (normal_y > 1 ) normal_y = 1;

	vec4 backgorund_texture_colour = texture(BackgroundTexture, tiled_coords) * back_texture_amount * normal_y + ( texture(RockTexture, tiled_coords * 0.5) * back_texture_amount * (1 - normal_y)) ;
	
	vec4 r_texture_colour = texture(rTexture, tiled_coords) * blend_map_colour.r;
	vec4 g_texture_colour = texture(gTexture, tiled_coords) * blend_map_colour.g;
	vec4 b_texture_colour = texture(bTexture, tiled_coords) * blend_map_colour.b;

	return backgorund_texture_colour + r_texture_colour + g_texture_colour + b_texture_colour ;
}

void main()
{
	//if (Distance > ViewDistance)
	//discard;

	WorldPosOut     = fs_in.position;
	DiffuseOut      = CalculateTerrainColor();
	NormalOut       = vec4(0.0f, 1.0f, 0.f, 1.f);
	SpecularOut     = vec4(0.f);
}
