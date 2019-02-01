#version 400
#define EPSILON 0.0002
struct SMaterial
{
    vec3  m_Ambient;
    vec3  m_Diffuse;
    vec3  m_Specular;
    float m_ShineDamper;
};
in vec2 TexCoord0;
in vec3 Normal0;
in vec4 WorldPos0; 

in vec3 PassTangent;
in float UseNormalMap;
in float FakeLight;

in vec4 ShadowCoords;
in float UseShadows;
in float ShadowMapSize;

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 MaterialSpecular;

uniform float     UseTexture;
uniform sampler2D gColorMap;
uniform sampler2D NormalMap;
uniform sampler2DShadow ShadowMap;

uniform SMaterial ModelMaterial;

in float Distance;
uniform float ViewDistance;

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
            vec3 UVC = vec3(ShadowCoords.xy + Offsets, ShadowCoords.z - EPSILON);
            
            //if (texture(ShadowMap, UVC) >  0.f)
                Factor += texture(ShadowMap, UVC);
           a++;
        }
    }
    float value = (0.5 + (Factor / a)) ;
    if( value > 1 )
    value =1 ;
    return value ;
}
vec3 CalcBumpedNormal(vec3 surface_normal, vec3 pass_tangent, sampler2D normal_map,vec2 text_coords)
{     
    vec3 normal = normalize(surface_normal);
    vec3 tangent = normalize(pass_tangent);
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 bitangent = cross(tangent, normal);
    vec3 bumpMapNormal = texture2D(normal_map, text_coords).xyz;
    bumpMapNormal = 2.0 * bumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 new_normal;
    mat3 tbn = mat3(tangent, bitangent, normal);
    new_normal = tbn * bumpMapNormal;
    new_normal = normalize(new_normal);
    return new_normal;
}  

void main()
{       
    if (Distance > ViewDistance)
        discard;

    vec4 texture_color = vec4(1.f, 1.f, 1.f, 1.f);
    if (UseTexture > .5f)
    {
        texture_color = texture(gColorMap, TexCoord0);
        if(texture_color.a < 0.5)
        {
            discard;
        }
    }   
    float shadow_factor = UseShadows > 0.5f ? CalculateShadowFactor() : 1.f;
    WorldPosOut      = WorldPos0;
    DiffuseOut       = texture_color * vec4(ModelMaterial.m_Diffuse, 1.0f) * shadow_factor; //vec4(vec3(0.8), 1.f); //

    vec3 normal;

    if (FakeLight < .5f)
        normal    = UseNormalMap > .5f ?  CalcBumpedNormal(Normal0, PassTangent, NormalMap, TexCoord0) :  normalize(Normal0);
    else
        normal    = normalize(Normal0);

    NormalOut = vec4(normal, 1.f); // w use fog
    MaterialSpecular = vec4(ModelMaterial.m_Specular, ModelMaterial.m_ShineDamper);
}
