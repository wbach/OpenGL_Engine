#pragma once
#include <string>
#include "../GpuObject.h"
#include "GameEngine/Api/IGraphicsApi.h"
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
    Texture(IGraphicsApi& graphicsApi)
        : graphicsApi_(graphicsApi)
    {
    }
    Texture(IGraphicsApi& graphicsApi, const std::string& file, const std::string& filepath,
             bool applySizeLimit = true);
    virtual ~Texture();
    inline uint32 GetId() const;
    inline const std::string GetFileName();
    inline const std::string GetFilPath();
    inline void SetExistId(uint32 i);
    inline bool IsInitialized() const;
    inline float GetTextureXOffset(uint32 textureIndex);
    inline float GetTextureYOffset(uint32 textureIndex);
    inline vec2 GetTextureOffset(uint32 textureIndex);

public:
    uint32 numberOfRows = 1;

protected:
    IGraphicsApi& graphicsApi_;
    std::string filename;
    std::string fullpath;

    bool applySizeLimit = true;

    uint32 id   = 0;
    bool isInit = false;
};

uint32 Texture::GetId() const
{
    return id;
}
const std::string Texture::GetFileName()
{
    return filename;
}
const std::string Texture::GetFilPath()
{
    return fullpath;
}
void Texture::SetExistId(uint32 i)
{
    id = i;
}
bool Texture::IsInitialized() const
{
    return isInit;
}
float Texture::GetTextureXOffset(uint32 textureIndex)
{
    return GameEngine::GetTextureXOffset(textureIndex, numberOfRows);
}
float Texture::GetTextureYOffset(uint32 textureIndex)
{
    return GameEngine::GetTextureYOffset(textureIndex, numberOfRows);
}
vec2 Texture::GetTextureOffset(uint32 textureIndex)
{
    return GameEngine::GetTextureOffset(textureIndex, numberOfRows);
}
}  // namespace GameEngine
