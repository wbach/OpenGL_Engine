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

void WebGpuTextureManager::DeleteTexture(uint32 textureId)
{
    textureInfo_.erase(textureId);
    images_.erase(textureId);
    imageArrays_.erase(textureId);
    rawData_.erase(textureId);
    activeTextureAssignments_.erase(textureId);
}

}  // namespace GraphicsApi::WebGpu
