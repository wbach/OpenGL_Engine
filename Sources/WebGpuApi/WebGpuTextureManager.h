#pragma once

#include <optional>
#include <unordered_map>
#include <vector>
#include <webgpu/webgpu.h>

#include "GraphicsApi/IGraphicsApi.h"

namespace GraphicsApi::WebGpu
{
class WebGpuTextureManager
{
public:
    void SetDevice(WGPUDevice device, WGPUQueue queue);

    ID CreateTexture(const Utils::Image& image, TextureFilter filter, TextureMipmap mipmap);
    ID CreateTexture(const std::vector<Utils::Image>& images, TextureFilter filter, TextureMipmap mipmap);
    ID CreateTextureStorage(TextureType type, TextureFilter filter, int32 samples);
    ID CreateCubMapTexture(const CubemapImages& cubemapImages);

    std::optional<uint64_t> GetBindlessHandle(IdType id);
    void GenerateMipmaps(IdType id);
    void CopyTexture(IdType sourceId, IdType targetId, uint32 xOffset, uint32 yOffset);

    void UpdateTexture(uint32 id, const vec2ui& offset, const Utils::Image& image);
    void UpdateTexture(uint32 id, const Utils::Image& image);
    void ClearTexture(uint32 id, const Color& color);

    void ActiveTexture(uint32 unit, uint32 textureId);
    void ActiveTexture(uint32 unit);
    void BindTexture(uint32 textureId) const;
    void BindImageTexture(uint32 textureId, TextureAccess access);

    std::vector<uint8> GetTextureData(uint32 textureId) const;
    const TextureInfo& GetTextureInfo(uint32 textureId) const;
    std::optional<Utils::Image> GetImage(IdType id) const;
    std::vector<Utils::Image> GetImageArray(IdType id) const;
    const std::unordered_map<uint32, uint32>& GetActiveTextureAssignments() const;
    std::optional<WGPUTextureView> GetTextureView(uint32 textureId) const;
    std::optional<WGPUSampler> GetSampler(uint32 textureId) const;

    void DeleteTexture(uint32 textureId);

public:
    struct TextureResource
    {
        WGPUTexture texture = nullptr;
        WGPUTextureView view = nullptr;
        WGPUSampler sampler = nullptr;
    };

private:

    mutable uint32 activeTextureUnit_ = 0;
    uint32 nextTextureId_             = 1;
    WGPUDevice device_                = nullptr;
    WGPUQueue queue_                 = nullptr;

    std::unordered_map<uint32, TextureInfo> textureInfo_;
    std::unordered_map<uint32, Utils::Image> images_;
    std::unordered_map<uint32, std::vector<Utils::Image>> imageArrays_;
    std::unordered_map<uint32, std::vector<uint8>> rawData_;
    std::unordered_map<uint32, TextureResource> gpuTextures_;
    mutable std::unordered_map<uint32, uint32> activeTextureAssignments_;
};

}  // namespace GraphicsApi::WebGpu
