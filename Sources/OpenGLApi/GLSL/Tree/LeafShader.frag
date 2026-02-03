#version 440
#define EPSILON 0.001

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 MaterialSpecular;

layout(binding = 0) uniform sampler2D BaseColorTexture;
layout(binding = 1) uniform sampler2D NormalTexture;
layout(binding = 2) uniform sampler2D RoughnessTexture;
layout(binding = 3) uniform sampler2D MetallicTexture;
layout(binding = 4) uniform sampler2D AmbientOcclusionTexture;
layout(binding = 5) uniform sampler2D OpacityTexture;
layout(binding = 6) uniform sampler2D DisplacementTexture;

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
    vec4 fogData; // xyz - color, w - gradient
} perApp;

layout(std140, align=16, binding = 4) uniform LeafParams
{
    vec4 wind; // xyz -dir, w - strength
    vec4 fparams; // x - leafScale, y - leafOffset, z - bendAmount
    ivec4 atlasParams; // x - atlasSize, y - atlasIndex
    float time;
} leafParams;

layout (std140, align=16, binding=6) uniform PerMaterial
{
    vec4 baseColor;
    vec4 params; // x - metallicFactor, y - roughnessFactor, z - ambientOcclusion, w - opacityCutoff
    vec4 params2; // x - normalScale,  y - useFakeLighting, z - specularStrength, w - indexOfRefraction
    vec4 hasTextures; // x - BaseColorTexture, y - NormalTexture, z -RoughnessTexture, w - MetallicTexture
    vec4 hasTextures2; // x - AmbientOcclusionTexture, y - OpacityTexture, z -DisplacementTexture, w - tiledScale
} perMaterial;

in VS_OUT
{
    vec2 texCoord;
    vec4 worldPos;
    vec3 normal;
    vec3 colorRandomness;
    flat int textureIndex;
} fs_in;

bool Is(float v)
{
    return v > 0.5f;
}

vec2 GetAtlasUV(vec2 uv, int idx)
{
    int cols = leafParams.atlasParams.x;
    int rows = leafParams.atlasParams.y;
    idx = clamp(idx, 0, cols*rows - 1);

    int col = idx % cols;
    int row = idx / cols;

    vec2 tileSize = vec2(1.0 / float(cols), 1.0 / float(rows));
    vec2 offset = vec2(float(col) * tileSize.x,
                       float(row) * tileSize.y);

    return offset + uv * tileSize;
}

float screenDoorAlpha(vec2 screenPos)
{
    const float bayer64[64] = float[](
         0, 32,  8, 40,  2, 34, 10, 42,
        48, 16, 56, 24, 50, 18, 58, 26,
        12, 44,  4, 36, 14, 46,  6, 38,
        60, 28, 52, 20, 62, 30, 54, 22,
         3, 35, 11, 43,  1, 33,  9, 41,
        51, 19, 59, 27, 49, 17, 57, 25,
        15, 47,  7, 39, 13, 45,  5, 37,
        63, 31, 55, 23, 61, 29, 53, 21
    );
    int x = int(screenPos.x) % 8;
    int y = int(screenPos.y) % 8;
    return bayer64[y * 8 + x] / 64.0;
}

void main()
{
    float t = leafParams.fparams.w;
    float threshold = screenDoorAlpha(gl_FragCoord.xy);
    if (t > threshold) discard;

    vec4 baseColor = vec4(1.0, 1.0, 1.0, 1.0);
    vec3 normal = fs_in.normal;
    
    float roughness = 1.0;
    float metallic = 0.0;
    float ao = 1.0;
    float displacement = 0.0;
    int textureIndex = fs_in.textureIndex;

    if (Is(perApp.useTextures.x))
    {
        baseColor  = texture(BaseColorTexture, GetAtlasUV(fs_in.texCoord, textureIndex));
        //baseColor = baseColor * vec4(0.95f, 1.00f, 0.95f, 1.0f); //perMaterial.baseColor;
        
        // Opacity
        vec4 opacityTex = texture(OpacityTexture, GetAtlasUV(fs_in.texCoord, textureIndex));
        if(opacityTex.x < 0.5) discard;
        baseColor.a = opacityTex.x;
    }

    if (Is(perApp.useTextures.y))
    {
        // Normal mapping
        normal = texture(NormalTexture, GetAtlasUV(fs_in.texCoord, textureIndex)).xyz * 2.0 - 1.0;
        normal = normalize(normal);
    }

    if (Is(perApp.useTextures.z))
    {
        // Roughness & Metallic from specular textures
        roughness = texture(RoughnessTexture, GetAtlasUV(fs_in.texCoord, textureIndex)).r;
        metallic  = texture(MetallicTexture, GetAtlasUV(fs_in.texCoord, textureIndex)).r;
    }

    // // Ambient Occlusion
    // ao = texture(AmbientOcclusionTexture, GetAtlasUV(fs_in.texCoord, textureIndex)).r;

    // Displacement
    if (Is(perApp.useTextures.w))
    {
        displacement = texture(DisplacementTexture, GetAtlasUV(fs_in.texCoord, textureIndex)).r;
    }

    vec3 albedo = baseColor.rgb ;//* fs_in.colorRandomness;
    albedo *= 0.9 + 0.1 * ao;

    WorldPosOut      = fs_in.worldPos + vec4(normal * displacement, 0.0);
    DiffuseOut = vec4(albedo, baseColor.a);
    NormalOut        = vec4(normal, 1.0);
    MaterialSpecular = vec4(metallic, roughness, 0.0, 1.0);
}
