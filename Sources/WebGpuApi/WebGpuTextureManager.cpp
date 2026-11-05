#include "WebGpuTextureManager.h"

#include <type_traits>
#include <variant>

namespace GraphicsApi::WebGpu
{
namespace
{
TextureInfo MakeTextureInfo(const Utils::Image* image = nullptr)
{
    TextureInfo info{};
    info.id = 0;

    if (image)
    {
        info.size.x = image->width;
        info.size.y = image->height;
        info.size.z = 1;
        info.textureType = TextureType::U8_RGBA;
    }

    return info;
}

void CreateGpuTextureResource(WGPUDevice device, WGPUQueue queue, uint32 textureId, const Utils::Image& image,
                             std::unordered_map<uint32, WebGpuTextureManager::TextureResource>& gpuTextures,
                             const std::unordered_map<uint32, std::vector<uint8>>& rawData)
{
    const auto imageData = rawData.at(textureId);
    if (imageData.empty())
    {
        return;
    }

    WGPUTextureDescriptor descriptor{};
    descriptor.dimension = WGPUTextureDimension_2D;
    descriptor.size.width = static_cast<uint32_t>(image.width);
    descriptor.size.height = static_cast<uint32_t>(image.height);
    descriptor.size.depthOrArrayLayers = 1;
    descriptor.sampleCount = 1;
    descriptor.format = WGPUTextureFormat_RGBA8Unorm;
    descriptor.mipLevelCount = 1;
    descriptor.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;

    WGPUTexture texture = wgpuDeviceCreateTexture(device, &descriptor);
    if (!texture)
    {
        return;
    }

    WGPUTextureViewDescriptor viewDescriptor{};
    viewDescriptor.label = WGPUStringView{"WebGPU Texture View", WGPU_STRLEN};
    viewDescriptor.format = WGPUTextureFormat_RGBA8Unorm;
    viewDescriptor.dimension = WGPUTextureViewDimension_2D;
    viewDescriptor.aspect = WGPUTextureAspect_All;
    viewDescriptor.baseMipLevel = 0;
    viewDescriptor.mipLevelCount = 1;
    viewDescriptor.baseArrayLayer = 0;
    viewDescriptor.arrayLayerCount = 1;
    auto view = wgpuTextureCreateView(texture, &viewDescriptor);

    WGPUSamplerDescriptor samplerDescriptor{};
    samplerDescriptor.label = WGPUStringView{"WebGPU Texture Sampler", WGPU_STRLEN};
    samplerDescriptor.addressModeU = WGPUAddressMode_Repeat;
    samplerDescriptor.addressModeV = WGPUAddressMode_Repeat;
    samplerDescriptor.addressModeW = WGPUAddressMode_Repeat;
    samplerDescriptor.magFilter = WGPUFilterMode_Linear;
    samplerDescriptor.minFilter = WGPUFilterMode_Linear;
    samplerDescriptor.mipmapFilter = WGPUMipmapFilterMode_Linear;
    samplerDescriptor.maxAnisotropy = 1;
    auto sampler = wgpuDeviceCreateSampler(device, &samplerDescriptor);

    if (view && sampler)
    {
        WGPUTexelCopyTextureInfo destination{};
        destination.texture = texture;
        destination.mipLevel = 0;
        destination.origin.x = 0;
        destination.origin.y = 0;
        destination.origin.z = 0;
        destination.aspect = WGPUTextureAspect_All;

        WGPUTexelCopyBufferLayout layout{};
        layout.bytesPerRow = static_cast<uint32_t>(image.width * 4);
        layout.rowsPerImage = static_cast<uint32_t>(image.height);

        WGPUExtent3D copySize{};
        copySize.width = static_cast<uint32_t>(image.width);
        copySize.height = static_cast<uint32_t>(image.height);
        copySize.depthOrArrayLayers = 1;

        wgpuQueueWriteTexture(queue, &destination, imageData.data(), imageData.size(), &layout, &copySize);
    }

    gpuTextures[textureId] = {texture, view, sampler};
}

std::vector<uint8> ExtractByteData(const Utils::Image& image)
{
    std::vector<uint8> bytes;
    const auto& data = image.getImageData();

    std::visit(
        [&](const auto& value)
        {
            using ValueType = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<ValueType, std::vector<uint8>>)
            {
                bytes = value;
            }
            else if constexpr (std::is_same_v<ValueType, std::vector<float>>)
            {
                bytes.reserve(value.size());
                for (const float sample : value)
                {
                    bytes.push_back(static_cast<uint8>(sample));
                }
            }
        },
        data);

    return bytes;
}
}  // namespace

ID WebGpuTextureManager::CreateTexture(const Utils::Image& image, TextureFilter filter, TextureMipmap mipmap)
{
    const uint32 textureId = nextTextureId_++;

    TextureInfo info = MakeTextureInfo(&image);
    info.id           = textureId;
    info.textureFilter = filter;
    info.textureMipmap = mipmap;

    textureInfo_[textureId] = info;
    images_[textureId]      = image;
    rawData_[textureId]     = ExtractByteData(image);
    activeTextureAssignments_[textureId] = 0;

    if (device_ && queue_ && !rawData_[textureId].empty())
    {
        CreateGpuTextureResource(device_, queue_, textureId, image, gpuTextures_, rawData_);
    }

    return textureId;
}

ID WebGpuTextureManager::CreateTexture(const std::vector<Utils::Image>& images, TextureFilter filter, TextureMipmap mipmap)
{
    const uint32 textureId = nextTextureId_++;

    TextureInfo info = MakeTextureInfo(images.empty() ? nullptr : &images.front());
    info.id           = textureId;
    info.textureFilter = filter;
    info.textureMipmap = mipmap;

    textureInfo_[textureId] = info;
    imageArrays_[textureId] = images;
    activeTextureAssignments_[textureId] = 0;

    return textureId;
}

ID WebGpuTextureManager::CreateTextureStorage(TextureType type, TextureFilter filter, int32)
{
    const uint32 textureId = nextTextureId_++;

    TextureInfo info{};
    info.id = textureId;
    info.textureType = type;
    info.textureFilter = filter;
    info.textureMipmap = TextureMipmap::NONE;

    textureInfo_[textureId] = info;
    activeTextureAssignments_[textureId] = 0;

    return textureId;
}

ID WebGpuTextureManager::CreateCubMapTexture(const CubemapImages& cubemapImages)
{
    const uint32 textureId = nextTextureId_++;

    TextureInfo info{};
    info.id = textureId;
    info.textureType = TextureType::U8_RGBA;
    textureInfo_[textureId] = info;

    imageArrays_[textureId] = {cubemapImages[0], cubemapImages[1], cubemapImages[2], cubemapImages[3],
                               cubemapImages[4], cubemapImages[5]};
    activeTextureAssignments_[textureId] = 0;

    return textureId;
}

std::optional<uint64_t> WebGpuTextureManager::GetBindlessHandle(IdType)
{
    return std::nullopt;
}

void WebGpuTextureManager::GenerateMipmaps(IdType)
{
}

void WebGpuTextureManager::CopyTexture(IdType, IdType, uint32, uint32)
{
}

void WebGpuTextureManager::UpdateTexture(uint32 id, const vec2ui& offset, const Utils::Image& image)
{
    images_[id] = image;
    rawData_[id] = ExtractByteData(image);

    auto infoIt = textureInfo_.find(id);
    if (infoIt != textureInfo_.end())
    {
        infoIt->second.id = id;
        infoIt->second.size.x = image.width;
        infoIt->second.size.y = image.height;
        infoIt->second.size.z = 1;
    }

    (void)offset;
}

void WebGpuTextureManager::UpdateTexture(uint32 id, const Utils::Image& image)
{
    UpdateTexture(id, {0u, 0u}, image);
}

void WebGpuTextureManager::ClearTexture(uint32 id, const Color& color)
{
    auto imageIt = images_.find(id);
    if (imageIt != images_.end())
    {
        imageIt->second.clearData();
    }

    (void)color;
}

void WebGpuTextureManager::ActiveTexture(uint32 unit, uint32 textureId)
{
    activeTextureAssignments_[unit] = textureId;
    activeTextureUnit_ = unit;
}

void WebGpuTextureManager::ActiveTexture(uint32 unit)
{
    activeTextureUnit_ = unit;
}

void WebGpuTextureManager::BindTexture(uint32 textureId) const
{
    activeTextureAssignments_[activeTextureUnit_] = textureId;
}

void WebGpuTextureManager::BindImageTexture(uint32 textureId, TextureAccess)
{
    activeTextureAssignments_[activeTextureUnit_] = textureId;
}

std::vector<uint8> WebGpuTextureManager::GetTextureData(uint32 textureId) const
{
    const auto rawIt = rawData_.find(textureId);
    if (rawIt != rawData_.end())
    {
        return rawIt->second;
    }

    const auto imageIt = images_.find(textureId);
    if (imageIt != images_.end())
    {
        return ExtractByteData(imageIt->second);
    }

    return {};
}

const TextureInfo& WebGpuTextureManager::GetTextureInfo(uint32 textureId) const
{
    static TextureInfo emptyInfo{};
    const auto it = textureInfo_.find(textureId);
    if (it != textureInfo_.end())
    {
        return it->second;
    }

    return emptyInfo;
}

std::optional<Utils::Image> WebGpuTextureManager::GetImage(IdType id) const
{
    const auto it = images_.find(id);
    if (it != images_.end())
    {
        return it->second;
    }

    return std::nullopt;
}

std::vector<Utils::Image> WebGpuTextureManager::GetImageArray(IdType id) const
{
    const auto it = imageArrays_.find(id);
    if (it != imageArrays_.end())
    {
        return it->second;
    }

    return {};
}

const std::unordered_map<uint32, uint32>& WebGpuTextureManager::GetActiveTextureAssignments() const
{
    return activeTextureAssignments_;
}

std::optional<WGPUTextureView> WebGpuTextureManager::GetTextureView(uint32 textureId) const
{
    const auto it = gpuTextures_.find(textureId);
    if (it == gpuTextures_.end() || !it->second.view)
    {
        return std::nullopt;
    }

    return it->second.view;
}

std::optional<WGPUSampler> WebGpuTextureManager::GetSampler(uint32 textureId) const
{
    const auto it = gpuTextures_.find(textureId);
    if (it == gpuTextures_.end() || !it->second.sampler)
    {
        return std::nullopt;
    }

    return it->second.sampler;
}

void WebGpuTextureManager::SetDevice(WGPUDevice device, WGPUQueue queue)
{
    device_ = device;
    queue_  = queue;
}

void WebGpuTextureManager::DeleteTexture(uint32 textureId)
{
    for (auto it = activeTextureAssignments_.begin(); it != activeTextureAssignments_.end();)
    {
        if (it->second == textureId)
        {
            it = activeTextureAssignments_.erase(it);
        }
        else
        {
            ++it;
        }
    }

    const auto it = gpuTextures_.find(textureId);
    if (it != gpuTextures_.end())
    {
        if (it->second.view)
        {
            wgpuTextureViewRelease(it->second.view);
        }
        if (it->second.sampler)
        {
            wgpuSamplerRelease(it->second.sampler);
        }
        if (it->second.texture)
        {
            wgpuTextureRelease(it->second.texture);
        }
        gpuTextures_.erase(it);
    }

    textureInfo_.erase(textureId);
    images_.erase(textureId);
    imageArrays_.erase(textureId);
    rawData_.erase(textureId);
}

}  // namespace GraphicsApi::WebGpu
