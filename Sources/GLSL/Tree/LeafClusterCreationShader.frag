#version 450 core

layout(location = 0) out vec4 outAlbedo; 
layout(location = 1) out vec4 outNormal; 

in VS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec3 colorRandomness;
    flat int textureIndex;
} fs_in;

layout(binding = 0) uniform sampler2D BaseColorTexture;
layout(binding = 1) uniform sampler2D NormalTexture;
layout(binding = 5) uniform sampler2D OpacityTexture;

layout(std140, align=16, binding = 4) uniform LeafParams
{
    vec4 wind; 
    vec4 fparams; 
    ivec4 atlasParams; 
    float time;
} leafParams;

vec2 GetAtlasUV(vec2 uv, int idx)
{
    int cols = 3;//leafParams.atlasParams.x;
    int rows = 3;//leafParams.atlasParams.x;
    idx = clamp(idx, 0, cols*rows - 1);

    int col = idx % cols;
    int row = idx / cols;

    vec2 tileSize = vec2(1.0 / float(cols), 1.0 / float(rows));
    vec2 offset = vec2(float(col) * tileSize.x,
                       float(row) * tileSize.y);

    return offset + uv * tileSize;
}

void main()
{
    int textureIndex = fs_in.textureIndex;
    vec2 uv = GetAtlasUV(fs_in.texCoord, textureIndex);

    vec4 opacityTex = texture(OpacityTexture, uv);
    if(opacityTex.r < 0.5) discard;

    vec4 texColor = texture(BaseColorTexture, uv);
    if (texColor.a < 0.1) discard;
    
    vec3 finalColor = texColor.rgb;// * fs_in.colorRandomness;
    finalColor *= vec3(0.95, 1.0, 0.95);
    vec3 detailNormal = texture(NormalTexture, uv).xyz * 2.0 - 1.0;
    vec3 worldNormal = normalize(fs_in.normal + detailNormal * 0.2); 

    outAlbedo = vec4(finalColor, texColor.a);
    outNormal = vec4(worldNormal * 0.5 + 0.5, 1.0);
}