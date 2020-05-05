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
    Texture(GraphicsApi::IGraphicsApi& graphicsApi, const GraphicsApi::ID& id);
    Texture(GraphicsApi::IGraphicsApi& graphicsApi, const std::string& file, const std::string& filepath,
           const vec2ui& size, bool applySizeLimit = true);
    ~Texture() override;
    void GpuLoadingPass() override;
    void ReleaseGpuPass() override;

    inline const vec2ui& GetSize() const;
    inline const std::string& GetFileName() const;
    inline const std::string& GetFilPath() const;
    inline float GetTextureXOffset(uint32 textureIndex) const;
    inline float GetTextureYOffset(uint32 textureIndex) const;
    inline vec2 GetTextureOffset(uint32 textureIndex) const;
    std::optional<uint32> GetNumberOfRowsBasedOnTextureFileName(const std::string& file) const;
    bool IsModified() const;
    void ResetModifyAcknowlage();

public:
    uint32 numberOfRows = 1;

protected:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    std::string filename;
    std::string fullpath;
    vec2ui size_ = vec2ui(0);

    bool applySizeLimit = true;
    bool orginalData_   = true;
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
