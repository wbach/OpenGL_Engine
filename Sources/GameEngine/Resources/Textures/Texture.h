#pragma once
#include <string>
#include "../GpuObject.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "TextureFlip.h"
#include "Types.h"

namespace GameEngine
{
float GetTextureXOffset(uint32 textureIndex, uint32 numberOfRows);
float GetTextureYOffset(uint32 textureIndex, uint32 numberOfRows);
vec2 GetTextureOffset(uint32 textureIndex, uint32 numberOfRows);

class Texture : public GpuObject
{
public:
    Texture(GraphicsApi::IGraphicsApi& graphicsApi);
    Texture(GraphicsApi::IGraphicsApi& graphicsApi, uint32 id);
    Texture(GraphicsApi::IGraphicsApi& graphicsApi, const std::string& file, const std::string& filepath,
            bool applySizeLimit = true);
    virtual ~Texture();

    inline const vec2ui& GetSize() const;
    inline const std::string& GetFileName() const;
    inline const std::string& GetFilPath() const;
    inline float GetTextureXOffset(uint32 textureIndex) const;
    inline float GetTextureYOffset(uint32 textureIndex) const;
    inline vec2 GetTextureOffset(uint32 textureIndex) const;
    std::optional<uint32> GetNumberOfRowsBasedOnTextureFileName(const std::string& file) const;

public:
    uint32 numberOfRows = 1;

protected:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    std::string filename;
    std::string fullpath;
    vec2ui size_ = vec2ui(0);

    bool applySizeLimit = true;
};

const std::string& Texture::GetFileName() const
{
    return filename;
}
const std::string& Texture::GetFilPath() const
{
    return fullpath;
}

float Texture::GetTextureXOffset(uint32 textureIndex) const
{
    return GameEngine::GetTextureXOffset(textureIndex, numberOfRows);
}
float Texture::GetTextureYOffset(uint32 textureIndex) const
{
    return GameEngine::GetTextureYOffset(textureIndex, numberOfRows);
}
vec2 Texture::GetTextureOffset(uint32 textureIndex) const
{
    return GameEngine::GetTextureOffset(textureIndex, numberOfRows);
}
const vec2ui& Texture::GetSize() const
{
    return size_;
}
}  // namespace GameEngine
