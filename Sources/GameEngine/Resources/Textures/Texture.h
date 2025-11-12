#pragma once
#include <string>

#include "../GpuObject.h"
#include "GameEngine/Resources/File.h"
#include "GameEngine/Resources/TextureParameters.h"
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
    Texture(GraphicsApi::IGraphicsApi&, const TextureParameters&);
    Texture(GraphicsApi::IGraphicsApi&, const TextureParameters&, const GraphicsApi::ID& id);
    Texture(GraphicsApi::IGraphicsApi&, const TextureParameters&, const vec2ui&, const std::optional<File>&);
    Texture(const Texture&) = delete;
    ~Texture() override;
    void GpuLoadingPass() override;
    void ReleaseGpuPass() override;

    void SetFile(const File&);
    inline const vec2ui& GetSize() const;
    inline const std::optional<File>& GetFile() const;
    inline float GetTextureXOffset(uint32 textureIndex) const;
    inline float GetTextureYOffset(uint32 textureIndex) const;
    inline vec2 GetTextureOffset(uint32 textureIndex) const;
    std::optional<uint32> GetNumberOfRowsBasedOnTextureFileName(const std::string& file) const;
    uint32 getNumberOfRows() const;
    const TextureParameters& getTextureParameters() const;

protected:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    TextureParameters textureParamters_;
    std::optional<File> file_;
    vec2ui size_;
    uint32 numberOfRows_;
};

const std::optional<File>& Texture::GetFile() const
{
    return file_;
}

float Texture::GetTextureXOffset(uint32 textureIndex) const
{
    return GameEngine::GetTextureXOffset(textureIndex, numberOfRows_);
}
float Texture::GetTextureYOffset(uint32 textureIndex) const
{
    return GameEngine::GetTextureYOffset(textureIndex, numberOfRows_);
}
vec2 Texture::GetTextureOffset(uint32 textureIndex) const
{
    return GameEngine::GetTextureOffset(textureIndex, numberOfRows_);
}
const vec2ui& Texture::GetSize() const
{
    return size_;
}
}  // namespace GameEngine
