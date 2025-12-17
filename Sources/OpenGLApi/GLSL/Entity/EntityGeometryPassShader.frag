#version 440 core

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
    vec4 fogData; // xyz - color, w - gradient
} perApp;

layout (std140, align=16, binding=6) uniform PerMaterial
{
    vec4 baseColor;
    vec4 params; // x - metallicFactor, y - roughnessFactor, z - ambientOcclusion, w - opacityCutoff
    vec4 params2; // x - normalScale,  y - useFakeLighting, z - specularStrength, w - indexOfRefraction
    vec4 hasTextures; // x - BaseColorTexture, y - NormalTexture, z -RoughnessTexture, w - MetallicTexture
    vec4 hasTextures2; // x - AmbientOcclusionTexture, y - OpacityTexture, z -DisplacementTexture, w - tiledScale
} perMaterial;

layout(location = 0) out vec4 WorldPosOut;
layout(location = 1) out vec4 ColorMapOut;
layout(location = 2) out vec4 NormalOut;
layout(location = 3) out vec4 SurfaceParamsOut;

layout(binding = 0) uniform sampler2D BaseColorTexture;
layout(binding = 1) uniform sampler2D NormalTexture;
layout(binding = 2) uniform sampler2D RoughnessTexture;
layout(binding = 3) uniform sampler2D MetallicTexture;
layout(binding = 4) uniform sampler2D AmbientOcclusionTexture;
layout(binding = 5) uniform sampler2D OpacityTexture;

in VS_OUT
{
    vec2 textureOffset;
    vec2 texCoord;
    vec3 normal;
    vec4 worldPos;
    mat3 tbn;
    float depth;
} fs_in;

bool Is(float v)
{
    return v > 0.5f;
}

bool IsTransparent(float opacity)
{
    const float opacityCutoff = perMaterial.params.w;
    return opacity < opacityCutoff;
}

bool NormalMaping()
{
    return Is(perApp.useTextures.y) && (fs_in.depth < perApp.viewDistance.y);
}

vec3 CalcBumpedNormal(vec2 text_coords)
{
    vec3 bumpMapNormal = texture(NormalTexture, text_coords).xyz;
    bumpMapNormal = bumpMapNormal * 2.f - 1.f;
    return normalize(fs_in.tbn * bumpMapNormal);
}

vec3 GetNormal(vec2 uv)
{
    const float useFakeLighting = perMaterial.params2.y;
    if(Is(useFakeLighting))
    {
        return vec3(0.0, 1.0, 0.0);
    }

    vec3 n = vec3(0.0, 1.0, 0.0);
    const bool hasNormalTexture = Is(perMaterial.hasTextures.y);
    if(hasNormalTexture && NormalMaping())
    {
        n = CalcBumpedNormal(uv);
    }
    else
    {
        n = normalize(fs_in.normal);
    }

    const float normalScale = perMaterial.params2.x;

    return normalize(n * normalScale);
}

void main()
{
    vec2 uv = fs_in.texCoord * perMaterial.hasTextures2.w;

    if(Is(perMaterial.hasTextures2.y))
    {
        const float opacity = texture(OpacityTexture, uv).r;
        if(IsTransparent(opacity)) discard;
    }

    // --- BaseColor + Opacity ---
    vec3 baseColor = perMaterial.baseColor.rgb;
    const bool hasBaseColorTexture = perMaterial.hasTextures.x > 0.5f;
    if(hasBaseColorTexture && perApp.useTextures.x > 0.5f)
    {
       baseColor = texture(BaseColorTexture, uv).rgb;
    }

    // --- Ambient Occlusion ---
    float ambientOcclusion = perMaterial.params.z;
    const bool hasAmbientOcclusionTexture = perMaterial.hasTextures2.x > 0.5f;
    if(hasAmbientOcclusionTexture)
    {
       ambientOcclusion = texture(AmbientOcclusionTexture, uv).r;
    }

    // --- Normal ---
    const vec3 normal = GetNormal(uv);

    float roughness = perMaterial.params.y;
    const bool hasRoughnessTexture = perMaterial.hasTextures.z > 0.5f;
    if(hasRoughnessTexture)
    {
       roughness = texture(RoughnessTexture, uv).r;
    }

    float metallic = perMaterial.params.x;
    const bool hasMetallicTexture = perMaterial.hasTextures.w > 0.5f;
    if(hasMetallicTexture)
    {
       metallic = texture(MetallicTexture, uv).r;
    }

    // --- Output do G-Buffer ---
    WorldPosOut         = fs_in.worldPos; // w - linear depth from vs
    ColorMapOut         = vec4(baseColor, ambientOcclusion);                       
    NormalOut           = vec4(normal, 1.0);                
    //SurfaceParamsOut    = vec4(metallic, roughness, perMaterial.params2.z, perMaterial.params2.w); 
    SurfaceParamsOut    = vec4(metallic, roughness, 1.f, perMaterial.params2.w); 
}
