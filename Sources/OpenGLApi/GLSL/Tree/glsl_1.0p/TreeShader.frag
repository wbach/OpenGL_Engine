#version 140

struct SMaterial
{
    vec3  ambient;
    vec3  diffuse;
    vec3  specular;
    float shineDamper;
};

in vec4 position;
in vec2 textureCoords;
in vec3 normal;

uniform SMaterial ModelMaterial;
uniform sampler2D DiffuseTexture;
uniform float UseShading;

out vec4 outputColor;

void main(void)
{
    vec4 color = texture(DiffuseTexture, textureCoords);
    color.xyz = color.xyz * 0.8f;
    if ( color.a < 0.5f)
    {
        discard;
    }

    outputColor = color * vec4(ModelMaterial.diffuse, 1.0f);
}