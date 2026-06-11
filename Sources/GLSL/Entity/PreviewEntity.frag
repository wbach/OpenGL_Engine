#version 440
#define EPSILON 0.0002

in VS_OUT
{
    vec2 texCoord;
    vec3 normal;
} vs_in;

uniform sampler2D DiffuseTexture;

out vec4 outputColor;

bool Is(float v)
{
    return v > 0.5f;
}

void main()
{
    vec4 colorFromTexture = vec4(1.f, 1.f, 1.f, 1.f);
    colorFromTexture = texture(DiffuseTexture, vs_in.texCoord);
    if(!Is(colorFromTexture.a))
    {
        discard;
    }

    float diffuseFactor = dot(vs_in.normal, normalize(vec3(100, 100, 100)));
    outputColor = diffuseFactor * vec4(0.5f); // * colorFromTexture
    outputColor.a = colorFromTexture.a;
}
