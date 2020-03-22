#version 430
#define TEXTURE_TILED_FACTOR 800.f
#define LARGE_DETAIL_RANGE 325.f
//#define scaleXZ 6000.f

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out; // line_strip

layout (std140, binding = 1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    mat4 toShadowMapSpace;
    vec3 cameraPosition;
} perFrame;

layout (std140, binding = 3) uniform PerTerrain
{
    vec4 displacementStrength;
    ivec4 morpharea1_4;
    ivec4 morpharea5_8;
    vec3 scale;
} perTerrain;

in vec2 mapCoord_GS[];

layout(binding = 2) uniform sampler2D blendMap;
layout(binding = 6) uniform sampler2D backgorundTextureDisplacement;
layout(binding = 9) uniform sampler2D redTextureDisplacement;
layout(binding = 12) uniform sampler2D greenTextureDisplacement;
layout(binding = 15) uniform sampler2D blueTextureDisplacement;

out vec4 worldPos;
out vec2 mapCoord_FS;
out vec3 tangent;

struct Displacement
{
    vec3 array[3];
};

vec3 calcTangent()
{
    vec3 v0 = gl_in[0].gl_Position.xyz;
    vec3 v1 = gl_in[1].gl_Position.xyz;
    vec3 v2 = gl_in[2].gl_Position.xyz;

    vec3 e1 = v1 - v0;
    vec3 e2 = v2 - v0;

    vec2 uv0 = mapCoord_GS[0];
    vec2 uv1 = mapCoord_GS[1];
    vec2 uv2 = mapCoord_GS[2];

    vec2 deltaUV1 = uv1 - uv0;
    vec2 deltaUV2 = uv2 - uv0;

    float r = 1.0 / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

    return normalize((e1 * deltaUV2.y - e2 * deltaUV1.y) * r);
}

Displacement calculateDisplacment()
{
    Displacement displacement;

    for(int k = 0; k < gl_in.length(); k++)
    {
        vec2 mapCoords = (gl_in[k].gl_Position.xz + perTerrain.scale.x / 2.f) / perTerrain.scale.x;
        vec2 tiledCoords = mapCoords * TEXTURE_TILED_FACTOR;

        vec4 blendMapColor = texture(blendMap, mapCoords);
        float backTextureAmount = 1.f - (blendMapColor.r + blendMapColor.g + blendMapColor.b) ;

        displacement.array[k] = vec3(0, 1, 0);

        float height = gl_in[k].gl_Position.y;

        float scale = 0.f;
        scale += texture(backgorundTextureDisplacement, tiledCoords).r * backTextureAmount * perTerrain.displacementStrength.x;
        scale += texture(redTextureDisplacement, tiledCoords).r * blendMapColor.r * perTerrain.displacementStrength.y;
        scale += texture(greenTextureDisplacement, tiledCoords).r * blendMapColor.g * perTerrain.displacementStrength.z;
        scale += texture(backgorundTextureDisplacement, tiledCoords).r * blendMapColor.b * perTerrain.displacementStrength.w;

        float attenuation = clamp(- distance(gl_in[k].gl_Position.xyz, perFrame.cameraPosition) / LARGE_DETAIL_RANGE + 1.f , 0.f, 1.f);
       // scale *= attenuation;

        displacement.array[k] *= scale;
    }

    return displacement;
}

void main()
{
    float dist = (distance(gl_in[0].gl_Position.xyz, perFrame.cameraPosition) + distance(gl_in[1].gl_Position.xyz, perFrame.cameraPosition) + distance(gl_in[2].gl_Position.xyz, perFrame.cameraPosition)) / 3.f;

    Displacement displacement;
    for (int i = 0; i < gl_in.length(); ++i)
    {
        displacement.array[i] = vec3(0);
    }

    if (dist < LARGE_DETAIL_RANGE)
    {
        tangent = calcTangent();
        displacement = calculateDisplacment();
    }

    for (int i = 0; i < gl_in.length(); ++i)
    {
        vec4 position = gl_in[i].gl_Position + vec4(displacement.array[i], 0);
        mapCoord_FS = mapCoord_GS[i];
        worldPos =  perFrame.projectionViewMatrix * position;
        gl_Position = worldPos;
        EmitVertex();
    }
    EndPrimitive();
}