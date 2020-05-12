#include "TextureLoader.h"

#include <FreeImage.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <algorithm>
#include <array>
#include <fstream>

#include "GLM/GLMUtils.h"
#include "GameEngine/Engine/Configuration.h"
#include "GpuResourceLoader.h"
#include "IGpuResourceLoader.h"
#include "Logger/Log.h"
#include "Textures/CubeMapTexture.h"
#include "Textures/GeneralTexture.h"
#include "Textures/HeightMap.h"
#include "Textures/MaterialTexture.h"
#include "Textures/NormalTexture.h"
#include "ResourceUtils.h"
#include "HeightMapHeader.h"

namespace GameEngine
{
namespace
{
uint32 unknownTextureNameId{0};
}

const std::string HEIGHTMAP_EXTENSION = "terrain";

TextureLoader::TextureLoader(GraphicsApi::IGraphicsApi& graphicsApi, IGpuResourceLoader& gpuLoader,
                             std::unordered_map<std::string, ResourceInfo<Texture>>& textures)
    : graphicsApi_(graphicsApi)
    , gpuResourceLoader_(gpuLoader)
    , textures_(textures)
    , textureNotFound_({nullptr, false})
{
}
TextureLoader::~TextureLoader()
{
}
Texture* TextureLoader::CreateTexture(const std::string& name, const TextureParameters& params, const TextureSize& size,
                                      RawData data)
{
    std::lock_guard<std::mutex> lk(textureMutex_);

    if (auto texture = GetTextureIfLoaded(name, params))
        return texture;

    return AddTexture(name, std::make_unique<GeneralTexture>(graphicsApi_, name, size, data), params.loadType);
}
Texture* TextureLoader::LoadTexture(const InputFileName& inputFileName, const TextureParameters& params)
{
    std::lock_guard<std::mutex> lk(textureMutex_);

    if (inputFileName.empty())
        return nullptr;

    if (auto texture = GetTextureIfLoaded(inputFileName, params))
        return texture;

    auto image = ReadFile(EngineConf_GetFullDataPathAddToRequierd(inputFileName), params);

    if (not image)
        return GetTextureNotFound();

    auto materialTexture = std::make_unique<MaterialTexture>(graphicsApi_, params.keepData, Utils::GetFilename(inputFileName),
                                                             Utils::GetFilePath(inputFileName), *image);

    return AddTexture(inputFileName, std::move(materialTexture), params.loadType);
}
Texture* TextureLoader::LoadCubeMap(const std::array<InputFileName, 6>& files, const TextureParameters& params)
{
    std::lock_guard<std::mutex> lk(textureMutex_);

    std::ostringstream textureName;
    for (const auto& file : files)
        textureName << file;

    if (auto texture = GetTextureIfLoaded(textureName.str(), params))
        return texture;

    std::array<Image, 6> images;

    size_t index = 0;
    for (const auto& file : files)
    {
        auto image = ReadFile(EngineConf_GetFullDataPathAddToRequierd(file), params);

        if (not image)
            return GetTextureNotFound();

        images[index++] = std::move(*image);
    }

    return AddTexture(textureName.str(), std::make_unique<CubeMapTexture>(graphicsApi_, textureName.str(), images, params.keepData),
                      params.loadType);
}
Texture* TextureLoader::LoadHeightMap(const InputFileName& inputFileName, const TextureParameters& params)
{
    std::lock_guard<std::mutex> lk(textureMutex_);

    if (auto texture = GetTextureIfLoaded(inputFileName, params))
    {
        return texture;
    }

    auto isBinnary = Utils::CheckExtension(inputFileName, HEIGHTMAP_EXTENSION);
    return isBinnary ? LoadHeightMapBinary(inputFileName, params) : LoadHeightMapTexture(inputFileName, params);
}
Texture* TextureLoader::CreateNormalMap(const HeightMap& heightMap, const vec3& terrainScale)
{
    std::lock_guard<std::mutex> lk(textureMutex_);

    auto normaltexture = CreateNormalTexture(graphicsApi_, terrainScale, heightMap);
    return AddTexture(GetNoName(), std::move(normaltexture), TextureLoadType::AddToGpuPass);
}
GraphicsApi::IGraphicsApi& TextureLoader::GetGraphicsApi()
{
    return graphicsApi_;
}
void TextureLoader::SaveTextureToFile(const std::string& name, const std::vector<uint8>& data, const vec2ui& size,
                                      uint8 bytes, GraphicsApi::TextureFormat format) const
{
    auto bytesData = const_cast<uint8*>(&data[0]);
    auto im        = FreeImage_ConvertFromRawBits(bytesData, static_cast<int>(size.x), static_cast<int>(size.y),
                                           static_cast<int>(bytes * size.x), 8 * bytes, 0, 0, 0);

    FREE_IMAGE_FORMAT fformat = FREE_IMAGE_FORMAT::FIF_PNG;
    std::string ext           = ".png";

    if (format == GraphicsApi::TextureFormat::BMP)
    {
        fformat = FREE_IMAGE_FORMAT::FIF_BMP;
        ext     = ".bmp";
    }

    if (format == GraphicsApi::TextureFormat::JPG)
    {
        fformat = FREE_IMAGE_FORMAT::FIF_JPEG;
        ext     = ".jpeg";
    }

    FreeImage_Save(fformat, im, (name + ext).c_str(), 0);
}

void TextureLoader::DeleteTexture(Texture& texture)
{
    std::lock_guard<std::mutex> lk(textureMutex_);

    auto iter = std::find_if(textures_.begin(), textures_.end(), [id = texture.GetGpuObjectId()](const auto& texture) {
        return (texture.second.resource_->GetGpuObjectId() == id);
    });

    auto& textureInfo = iter->second;
    --textureInfo.instances_;

    if (textureInfo.instances_ > 0)
        return;

    gpuResourceLoader_.AddObjectToRelease(std::move(textureInfo.resource_));
    textures_.erase(iter);
}

Texture* TextureLoader::LoadHeightMapBinary(const InputFileName& inputFileName, const TextureParameters& params)
{
    auto fp = fopen(inputFileName.c_str(), "rb");

    if (!fp)
    {
        ERROR_LOG("GetFileType: wrong image format or file does not exist : " + inputFileName);
        return nullptr;
    }

    HeightMapHeader header;
    auto bytes = fread(&header, sizeof(HeightMapHeader), 1, fp);

    if (bytes == 0)
    {
        ERROR_LOG("Read file error." + inputFileName);
    }

    DEBUG_LOG("Size : " + std::to_string(header.width) + "x" + std::to_string(header.height));
    DEBUG_LOG("Height map scale : " + std::to_string(header.scale));

    Image image;
    image.width  = header.width;
    image.height = header.height;

    auto size = header.width * header.height;
    image.floatData.resize(size);

    bytes = fread(&image.floatData[0], sizeof(float), size, fp);
    fclose(fp);

    if (bytes < size)
    {
        ERROR_LOG("Read file error." + inputFileName + " " + ", bytes : " + std::to_string(bytes) + "/" +
                  std::to_string(sizeof(float) * size));
        return nullptr;
    }

    auto heightmapTexture = std::make_unique<HeightMap>(graphicsApi_, Utils::GetFilename(inputFileName),
                                                        GetRelativeDataPath(inputFileName), std::move(image));
    heightmapTexture->SetScale(header.scale);

    return AddTexture(inputFileName, std::move(heightmapTexture), params.loadType);
}
Texture* TextureLoader::LoadHeightMapTexture(const InputFileName& inputFileName, const TextureParameters& params)
{
    DEBUG_LOG(inputFileName);

    auto optImage = ReadFile(inputFileName, params);

    if (not optImage)
    {
        return nullptr;
    }

    auto& image = *optImage;

    if (image.floatData.empty())
    {
        image.floatData.reserve(image.data.size());

        for (auto i = 0u; i < image.data.size(); i += 4)
        {
            auto color = (Utils::RGBtoFloat(image.data[i]) + Utils::RGBtoFloat(image.data[i + 1]) +
                          Utils::RGBtoFloat(image.data[i + 2])) /
                         3.f;
            image.floatData.push_back(color);
        }

        image.data.clear();
    }

    auto heightmapTexture = std::make_unique<HeightMap>(graphicsApi_, Utils::GetFilename(inputFileName),
                                                        GetRelativeDataPath(inputFileName), std::move(image));

    return AddTexture(inputFileName, std::move(heightmapTexture), params.loadType);
}

Texture* TextureLoader::GetTextureIfLoaded(const std::string& name, const TextureParameters& params)
{
    if (textures_.count(name))
    {
        DEBUG_LOG("Created texture already exist : " + name);

        auto& textureInfo = textures_.at(name);
        ++textureInfo.instances_;

        if (params.loadType != TextureLoadType::None and textureInfo.resourceGpuStatus_ == ResourceGpuStatus::NotLoaded)
        {
            if (params.loadType == TextureLoadType::Immediately)
            {
                textureInfo.resource_->GpuLoadingPass();
            }
            else if (params.loadType == TextureLoadType::AddToGpuPass)
            {
                gpuResourceLoader_.AddObjectToGpuLoadingPass(*textureInfo.resource_);
            }

            textureInfo.resourceGpuStatus_ = ResourceGpuStatus::Loaded;
        }

        return textureInfo.resource_.get();
    }

    return nullptr;
}

Texture* TextureLoader::GetTextureNotFound()
{
    if (textureNotFound_.second)
        return textureNotFound_.first;

    textureNotFound_.second = true;
    auto filename           = "Textures/textureNotFound.png";
    TextureParameters params;
    auto image = ReadFile(EngineConf_GetFullDataPathAddToRequierd(filename), params);

    if (image)
    {
        auto materialTexture   = std::make_unique<MaterialTexture>(graphicsApi_, false, Utils::GetFilename(filename),
                                                                 Utils::GetFilePath(filename), *image);
        textureNotFound_.first = AddTexture(filename, std::move(materialTexture), params.loadType);
    }

    return textureNotFound_.first;
}

Texture* TextureLoader::AddTexture(const std::string& name, std::unique_ptr<Texture> texture, TextureLoadType loadType)
{
    ResourceInfo<Texture> textureInfo;
    textureInfo.resourceGpuStatus_ = Convert(loadType);
    textureInfo.resource_          = std::move(texture);

    auto result = textureInfo.resource_.get();
    textures_.insert({name, std::move(textureInfo)});
    ApplyLoadTypeAction(*result, loadType);
    return result;
}

void TextureLoader::ApplyLoadTypeAction(Texture& texture, TextureLoadType type)
{
    switch (type)
    {
        case TextureLoadType::AddToGpuPass:
            gpuResourceLoader_.AddObjectToGpuLoadingPass(texture);
            break;
        case TextureLoadType::Immediately:
            texture.GpuLoadingPass();
            break;
        case TextureLoadType::None:
            return;
    }
}

std::string TextureLoader::GetNoName() const
{
    return "nonameTexture_" + std::to_string(unknownTextureNameId++);
}

ResourceGpuStatus TextureLoader::Convert(TextureLoadType loadType)
{
    return (loadType == TextureLoadType::None) ? ResourceGpuStatus::NotLoaded : ResourceGpuStatus::Loaded;
}
}  // namespace GameEngine
