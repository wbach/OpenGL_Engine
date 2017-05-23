#version 330
                                                                        
in vec2 TexCoord0;                                                                  
in vec3 Normal0;                                                                    
in vec4 WorldPos0;  
                                                                
in vec4 ShadowCoords;
in float UseShadows;
in float ShadowMapSize;

in vec3 PassTangent;
in float UseNormalMap;

layout (location = 0) out vec4 WorldPosOut;   
layout (location = 1) out vec4 DiffuseOut;     
layout (location = 2) out vec4 NormalOut;     
layout (location = 3) out vec4 SpecularOut;    

uniform sampler2D BlendMap ;									
uniform sampler2D BackgroundTexture ;
uniform sampler2D rTexture ;
uniform sampler2D gTexture ;
uniform sampler2D bTexture ;
uniform sampler2D BackgroundTextureNormal;
uniform sampler2D rTextureNormal ;
uniform sampler2D gTextureNormal ;
uniform sampler2D bTextureNormal ;
uniform sampler2DShadow  ShadowMap ;           

uniform sampler2D RockTexture;
uniform sampler2D RockTextureNormal;     
		
in float Distance;
uniform float ViewDistance;

vec3 CalcBumpedNormal(vec3 surface_normal, vec3 pass_tangent, vec4 normal_map)
{                                                                                           
    vec3 normal = normalize(surface_normal);                                                       
    vec3 tangent = normalize(pass_tangent);                                                     
    tangent = normalize(tangent - dot(tangent, normal) * normal);                           
    vec3 bitangent = cross(tangent, normal);  
    vec3 bumpMapNormal = normal_map.xyz ;                                
    bumpMapNormal = 2.0 * bumpMapNormal - vec3(1.0, 1.0, 1.0);                              
    vec3 new_normal;                                                                         
    mat3 tbn = mat3(tangent, bitangent, normal);                                            
    new_normal = tbn * bumpMapNormal;     
	new_normal.y *= 0.25f;
    new_normal = normalize(new_normal);                                                       
    return new_normal;                                                                       
}
float CalculateShadowFactor()
{
    float xOffset = 1.0/ShadowMapSize;
    float yOffset = 1.0/ShadowMapSize;

    float Factor = 0.0;

	float a = 0;
    for (int y = -1 ; y <= 1 ; y++) 
	{
        for (int x = -1 ; x <= 1 ; x++) 
		{
            vec2 Offsets = vec2(float(x) * xOffset, float(y) * yOffset);
            vec3 UVC = vec3(ShadowCoords.xy + Offsets, ShadowCoords.z);
			
			if (texture(ShadowMap, UVC) >  0.f)
				Factor += (ShadowCoords.w * 0.4f);
		   a++;
        }
    }
	float value = (0.5 + (Factor / a)) ;
	if( value > 1 )
	value = 1 ;
    return value ;
}
vec4 CalculateTerrainColor(inout vec4 normal)
{
	vec4 blend_map_colour = texture(BlendMap, TexCoord0) ;
		
	float back_texture_amount = 1 - (blend_map_colour.r + blend_map_colour.g + blend_map_colour.b) ;
	vec2 tiled_coords = TexCoord0 * 40.0f ;

	float normal_y = abs(normalize(Normal0).y);

	normal_y = normal_y * 2 ;

	if (normal_y > 1 ) normal_y = 1;

	vec4 backgorund_texture_colour = texture(BackgroundTexture, tiled_coords) * back_texture_amount * normal_y + ( texture(RockTexture, tiled_coords * 0.25) * back_texture_amount * (1 - normal_y)) ;
	
	vec4 r_texture_colour = texture(rTexture, tiled_coords) * blend_map_colour.r;
	vec4 g_texture_colour = texture(gTexture, tiled_coords) * blend_map_colour.g;
	vec4 b_texture_colour = texture(bTexture, tiled_coords) * blend_map_colour.b;
	
	if (UseNormalMap > .5f )
	{
		vec4 backgorund_n_texture_colour = texture(BackgroundTextureNormal, tiled_coords) * back_texture_amount * normal_y + ( texture(RockTextureNormal, tiled_coords * 0.25) * back_texture_amount * (1 - normal_y)) ;
		vec4 rn_texture_colour = texture(rTextureNormal, tiled_coords) * blend_map_colour.r;
		vec4 gn_texture_colour = texture(gTextureNormal, tiled_coords) * blend_map_colour.g;
		vec4 bn_texture_colour = texture(bTextureNormal, tiled_coords) * blend_map_colour.b;
		normal = vec4(CalcBumpedNormal(Normal0, PassTangent, backgorund_n_texture_colour + rn_texture_colour + gn_texture_colour + bn_texture_colour), 1.f); // w use fog
		// normal = vec4(normalize(Normal0), 1.f);
	}
	else
	{
	   normal = vec4(normalize(Normal0), 1.f);
	}	
	return backgorund_texture_colour + r_texture_colour + g_texture_colour + b_texture_colour;
}
											
void main()									
{	
	if (Distance > ViewDistance)
	discard;

	float shadow_factor = UseShadows > 0.5f ? CalculateShadowFactor() : 1.f;									
	WorldPosOut     = WorldPos0;					
	DiffuseOut      = CalculateTerrainColor(NormalOut) * shadow_factor;	
	SpecularOut     = vec4(0.f);
	//NormalOut 		= vec4(normal, 1.f);
}
