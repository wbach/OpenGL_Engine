#version 330
#define EPSILON 0.001

layout (location = 0) out vec3 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec3 MaterialSpecular;

uniform sampler2D Texture0;
uniform sampler2DShadow ShadowMap;

in vec2 TexCoord;
in vec3 WorldPos; 

in vec4 ShadowCoords;
in float UseShadows;
in float ShadowMapSize;

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
            vec3 UVC = vec3(ShadowCoords.xy + Offsets, ShadowCoords.z + EPSILON);
            
            if (texture(ShadowMap, UVC) >  0.f)
                Factor += (ShadowCoords.w * 0.4f);
           a++;
        }
    }
    float value = (0.5 + (Factor / a)) ;
    if( value > 1 )
    value =1 ;
    return value ;
}

void main()
{   
    vec4 texture_color  = texture(Texture0, TexCoord);
    if(texture_color.a < .5f) discard;
    
    float shadow_factor = UseShadows > 0.5f ? CalculateShadowFactor() : 1.0;
    
    WorldPosOut      = WorldPos;
    DiffuseOut       = texture_color * shadow_factor; //vec4(1.f, 0.f, 0.f, 1.f);
    NormalOut        = vec4(.0f, 1.f, .0f, 1.f);
    MaterialSpecular = vec3(.0f);
}