#version 440
#define EPSILON 0.001

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 MaterialSpecular;

uniform sampler2D Texture0;

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
    vec4 fogData; // xyz - color, w - gradient
} perApp;

layout(std140, align=16, binding = 4) uniform LeafParams
{
    vec4 fparams; // x -leafScale
    ivec4 atlasParams; // x - atlasSize, y - atlasIndex
} leafParams;

in GS_OUT
{
    vec2 texCoord;
    vec4 worldPos;
    vec3 normal;
    vec3 color;
} fs_in;

bool Is(float v)
{
    return v > 0.5f;
}

vec2 GetAtlasUV(vec2 uv, int idx)
{
    int cols = leafParams.atlasParams.x;
    int rows = leafParams.atlasParams.x;

    // clamp idx
    idx = clamp(idx, 0, cols*rows - 1);

    // oblicz kolumnę i wiersz
    int col = idx % cols;
    int row = idx / cols;

    // rozmiar jednej subtekstury
    vec2 tileSize = vec2(1.0 / float(cols), 1.0 / float(rows));

    // offset do odpowiedniej subtekstury
    vec2 offset = vec2(float(col) * tileSize.x,
                       float(row) * tileSize.y);

    // finalne UV
    return offset + uv * tileSize;
}

void main()
{
    vec4 diffTexture = vec4(1.f, 1.f, 1.f, 1.f);

    if (Is(perApp.useTextures.x))
    {
        diffTexture  = texture(Texture0, GetAtlasUV(fs_in.texCoord, leafParams.atlasParams.y));
        if(diffTexture.a < .5f) discard;
    }

    vec3 normal = fs_in.normal;
    WorldPosOut      = fs_in.worldPos;
    DiffuseOut       = diffTexture * vec4(fs_in.color, 0.5f);
    NormalOut        = vec4(normal, 1.f);
    MaterialSpecular = vec4(.0f);
}