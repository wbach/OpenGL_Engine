#version 330
#define EPSILON 0.002
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

layout (location = 0) out vec4 WorldPosOut;   
layout (location = 1) out vec4 DiffuseOut;     
layout (location = 2) out vec4 NormalOut;     
layout (location = 3) out vec4 MaterialSpecular;

uniform sampler2D gColorMap;

uniform SMaterial ModelMaterial;

in float Distance;
uniform float ViewDistance;

void main()									
{		
	if (Distance > ViewDistance)
	discard;

	vec4 texture_color = texture(gColorMap, TexCoord0);
	if(texture_color.a < 0.5)
    {   
        discard ;
    }
	
	WorldPosOut      = WorldPos0;					
	DiffuseOut       = texture_color;

	NormalOut = vec4(normalize(Normal0), 1.f); // w use fog
	MaterialSpecular = vec4(ModelMaterial.m_Specular, ModelMaterial.m_ShineDamper);
}
