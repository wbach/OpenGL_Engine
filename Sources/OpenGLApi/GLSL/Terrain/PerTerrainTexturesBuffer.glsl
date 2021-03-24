layout (std140, binding = 6) uniform PerTerrainTexturesBuffer
{
    vec4 rgbaTextureScales;
    vec4 backgroundTextureScales;
    vec4 haveTextureR;
    vec4 haveTextureG;
    vec4 haveTextureB;
    vec4 haveTextureA;
    vec4 haveTextureRock;
    vec4 haveTextureBackground;
    float haveBlendMap;
} perTerrainTextures;
