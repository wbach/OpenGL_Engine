#version 440

layout (std140, align=16, binding=6) uniform PerMeshObject
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    uint numberOfRows;
    float useTexture;
    float useNormalMap;
    float shineDamper;
    float useFakeLighting;
} perMeshObject;

in VS_OUT
{
    vec2 texCoord;
    vec2 textureOffset;
} vs_in;

uniform sampler2D ModelTexture;

out vec4 outputColor;

void main(void)
{
    vec2 textCoord = (vs_in.texCoord / perMeshObject.numberOfRows) + vs_in.textureOffset;
    float alpha = texture(ModelTexture, textCoord ).a;
    if ( alpha < 0.5f)
    {
        discard;
    }
    outputColor = vec4(1.0);
}