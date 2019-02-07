#version 320
#define EPSILON 0.0002

struct SMaterial
{
    vec3  m_Ambient;
    vec3  m_Diffuse;
    vec3  m_Specular;
    float m_ShineDamper;
};

in vec2 TexCoord0;
in float Distance;

uniform float     UseTexture;
uniform sampler2D gColorMap;
uniform float ViewDistance;
uniform SMaterial ModelMaterial;

out vec4 outputColor;

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

    outputColor = texture_color * vec4(ModelMaterial.m_Diffuse, 1.0f);
}
