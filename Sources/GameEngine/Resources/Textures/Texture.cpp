#include "Texture.h"
#include "Logger/Log.h"

namespace GameEngine
{
float GetTextureXOffset(uint32 textureIndex, uint32 numberOfRows)
{
    int column = textureIndex % numberOfRows;
    return static_cast<float>(column) / static_cast<float>(numberOfRows);
}
float GetTextureYOffset(uint32 textureIndex, uint32 numberOfRows)
{
    int row = textureIndex / numberOfRows;
    return static_cast<float>(row) / static_cast<float>(numberOfRows);
}
vec2 GetTextureOffset(uint32 textureIndex, uint32 numberOfRows)
{
    return vec2(GetTextureXOffset(textureIndex, numberOfRows), GetTextureYOffset(textureIndex, numberOfRows));
}

Texture::Texture(IGraphicsApi& graphicsApi, const std::string& file, const std::string& filepath, bool applySizeLimit)
    : graphicsApi_(graphicsApi)
    , filename(file)
    , fullpath(filepath)
    , applySizeLimit(applySizeLimit)
{
}
Texture::~Texture()
{
    if (!isInit)
        return;
    Log("Delete " + fullpath + ", texture id: " + std::to_string(id));

    graphicsApi_.DeleteObject(id);
}
}  // namespace GameEngine
