#version 330

in TES_OUT
{
    vec2 textCoord;
    vec3 world_coord;
    vec3 eye_coord;
    vec4 position;
    vec3 normal;
    float height;
    vec4 shadowCoords;
    float useShadows;
    float shadowMapSize;
} fs_in;

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 SpecularOut;

uniform sampler2DShadow shadowMap;
uniform sampler2D blendMap;
uniform sampler2D backgorundTexture;
uniform sampler2D backgorundTextureNormal;
uniform sampler2D redTexture;
uniform sampler2D redTextureNormal;
uniform sampler2D greenTexture;
uniform sampler2D greenTextureNormal;
uniform sampler2D blueTexture;
uniform sampler2D blueTextureNormal;
uniform sampler2D rockTexture;
uniform sampler2D rockNormalTexture;
uniform sampler2D snowTexture;
uniform sampler2D snowTextureNormal;

const float EPSILON = 0.005f;

vec4 CalculateColor(sampler2D textureId, float factor, vec2 textCoords, float normalFactor)
{
	vec4 baseColor = texture(textureId, textCoords) * factor * normalFactor;
	vec4 rockColor = texture(rockTexture, textCoords) * factor * (1.f - normalFactor);

	return  baseColor + rockColor;
}

float CalculateShadowFactor()
{
    float xOffset = 1.f / fs_in.shadowMapSize;
    float yOffset = 1.f / fs_in.shadowMapSize;

    float factor = 0.f;

	float a = 0.f;
    for (int y = -1 ; y <= 1 ; y++) 
	{
        for (int x = -1 ; x <= 1 ; x++) 
		{
            vec2 offsets = vec2(float(x) * xOffset, float(y) * yOffset);
            vec3 uvc = vec3(fs_in.shadowCoords.xy + offsets, fs_in.shadowCoords.z);
			
			if (texture(shadowMap, uvc) >  0.f)
				factor += (fs_in.shadowCoords.w * .4f);
		   ++a;
        }
    }
	float value = (0.5 + (factor / a)) ;
	if( value > 1 )
		value = 1 ;
    return value ;
}

vec4 CalculateTerrainColor()
{
	vec4 blend_map_colour = texture(blendMap, fs_in.textCoord) ;
		
	float back_texture_amount = 1 - (blend_map_colour.r + blend_map_colour.g + blend_map_colour.b) ;
	vec2 tiled_coords = fs_in.textCoord * 200.0f ;

	float normalFactor = dot(fs_in.normal, vec3(0.f, 4.f, 0.f));

	if (normalFactor > 1 )
	 normalFactor = 1;

	vec4 backgorund_texture_colour;
	backgorund_texture_colour = CalculateColor(backgorundTexture, back_texture_amount, tiled_coords, normalFactor);

	vec4 r_texture_colour = CalculateColor(redTexture, blend_map_colour.r, tiled_coords, normalFactor);
	vec4 g_texture_colour = CalculateColor(greenTexture, blend_map_colour.g, tiled_coords, normalFactor);
	vec4 b_texture_colour = CalculateColor(blueTexture, blend_map_colour.b, tiled_coords, normalFactor);

	return backgorund_texture_colour + r_texture_colour + g_texture_colour + b_texture_colour ;
}

bool OutOfHeightMapSize()
{
	return (fs_in.textCoord.x < EPSILON || fs_in.textCoord.x > (1.f - EPSILON) || fs_in.textCoord.y <EPSILON || fs_in.textCoord.y > (1.f - EPSILON));
}

void main()
{
	//if (Distance > ViewDistance)
	//discard;

	if (OutOfHeightMapSize())
		discard;

	float shadow_factor = CalculateShadowFactor();

	WorldPosOut     = fs_in.position;
	DiffuseOut      = CalculateTerrainColor() * shadow_factor;
	NormalOut       = vec4(fs_in.normal, 1.0f);//vec4(0.0f, 1.0f, 0.f, 1.f);
	SpecularOut     = vec4(0.f);
}
