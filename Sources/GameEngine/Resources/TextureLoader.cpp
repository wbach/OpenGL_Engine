#include "TextureLoader.h"

#include <FreeImage.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <algorithm>
#include <array>
#include <fstream>

#include "GLM/GLMUtils.h"
#include "GameEngine/Engine/Configuration.h"
#include "GpuResourceLoader.h"
#include "HeightMapHeader.h"
#include "IGpuResourceLoader.h"
#include "Logger/Log.h"
#include "ResourceUtils.h"
#include "Textures/CubeMapTexture.h"
#include "Textures/GeneralTexture.h"
#include "Textures/HeightMap.h"
#include "textureNotFound.h"

namespace GameEngine
{
namespace
{
uint32 unknownTextureNameId{0};
}
TextureLoader::TextureLoader(GraphicsApi::IGraphicsApi& graphicsApi, IGpuResourceLoader& gpuLoader)
    : graphicsApi_(graphicsApi)
    , gpuResourceLoader_(gpuLoader)
    , textureNotFound_({nullptr, false})
    , releaseLockState_(false)
{
}
TextureLoader::~TextureLoader()
{
    std::vector<Texture*> toRelease;
    for (auto& texture : textures_)
        toRelease.push_back(texture.second.resource_.get());
    DEBUG_LOG("Release not deleted textures. size :" + std::to_string(toRelease.size()));
    for (auto texture : toRelease)
        DeleteTexture(*texture);
}
GeneralTexture* TextureLoader::CreateTexture(const std::string& name, const TextureParameters& params,
                                             const Utils::Image& image)
{
    std::lock_guard<std::mutex> lk(textureMutex_);

    if (auto texture = GetTextureIfLoaded(name, params))
        return static_cast<GeneralTexture*>(texture);

    auto texture    = std::make_unique<GeneralTexture>(graphicsApi_, image, params);
    auto texturePtr = texture.get();
    AddTexture(name, std::move(texture), params.loadType);
    return texturePtr;
}
void TextureLoader::UpdateTexture(const GeneralTexture& texture)
{
    if (texture.GetGraphicsObjectId())
    {
        gpuResourceLoader_.AddFunctionToCall([texture = &texture, this]() {
            graphicsApi_.UpdateTexture(*texture->GetGraphicsObjectId(), texture->GetImage());
        });
    }
}
void TextureLoader::UpdateTexture(GeneralTexture*& texture, const std::string& newName)
{
    if (not textures_.empty())
    {
        auto iter =
            std::find_if(textures_.begin(), textures_.end(), [id = texture->GetGpuObjectId()](const auto& texture) {
                return (texture.second.resource_->GetGpuObjectId() == id);
            });

        texture = CreateTexture(newName, iter->second.resource_->getTextureParameters(), texture->GetImage());

        if (iter != textures_.end())
        {
            DeleteTexture(*iter->second.resource_);
        }
    }
}
GeneralTexture* TextureLoader::LoadTexture(const File& inputFileName, const TextureParameters& params)
{
    std::lock_guard<std::mutex> lk(textureMutex_);

    File inputFile = inputFileName;
    if (not inputFileName)
    {
        // WARNING_LOG("File not exist : " + inputFileName.GetAbsoultePath() + " try find it in data directory");
        auto filename = Utils::FindFile(inputFileName.GetFilename(), EngineConf.files.data);
        if (not filename.empty())
        {
            inputFile = File(filename);
            // DEBUG_LOG("Found texture in : " + inputFile.GetAbsoultePath());
        }
        else
        {
            ERROR_LOG("File not exist : " + inputFileName.GetAbsoultePath());
            return GetTextureNotFound();
        }
    }

    if (auto texture = GetTextureIfLoaded(inputFile.GetAbsoultePath(), params))
        return static_cast<GeneralTexture*>(texture);

    auto image = ReadFile(inputFile, params);

    if (not image)
        return GetTextureNotFound();

    auto texture    = std::make_unique<GeneralTexture>(graphicsApi_, *image, params, inputFile);
    auto texturePtr = texture.get();
    AddTexture(inputFile.GetAbsoultePath(), std::move(texture), params.loadType);
    return texturePtr;
}
CubeMapTexture* TextureLoader::LoadCubeMap(const std::array<File, 6>& files, const TextureParameters& params)
{
    std::lock_guard<std::mutex> lk(textureMutex_);

    std::ostringstream textureName;
    for (const auto& file : files)
        textureName << file;

    if (auto texture = GetTextureIfLoaded(textureName.str(), params))
        return static_cast<CubeMapTexture*>(texture);

    std::array<Utils::Image, 6> images;

    size_t index = 0;
    for (const auto& file : files)
    {
        auto image = ReadFile(file, params);

        if (not image)
            return nullptr;

        images[index++] = std::move(*image);
    }

    auto cubeMap    = std::make_unique<CubeMapTexture>(graphicsApi_, params, textureName.str(), images);
    auto cubeMapPtr = cubeMap.get();
    AddTexture(textureName.str(), std::move(cubeMap), params.loadType);
    return cubeMapPtr;
}
HeightMap* TextureLoader::LoadHeightMap(const File& inputFileName, const TextureParameters& params)
{
    std::lock_guard<std::mutex> lk(textureMutex_);

    if (auto texture = GetTextureIfLoaded(inputFileName.GetAbsoultePath(), params))
    {
        return static_cast<HeightMap*>(texture);
    }

    auto isBinnary = inputFileName.IsExtension(".terrain");
    return isBinnary ? LoadHeightMapBinary(inputFileName, params) : LoadHeightMapTexture(inputFileName, params);
}
GeneralTexture* TextureLoader::CreateNormalMap(const HeightMap& heightMap, const vec3& terrainScale)
{
    std::lock_guard<std::mutex> lk(textureMutex_);

    auto normaltexture    = CreateNormalTexture(graphicsApi_, terrainScale, heightMap);
    auto normaltexturePtr = normaltexture.get();
    AddTexture(GetNoName(), std::move(normaltexture), TextureLoadType::AddToGpuPass);
    return normaltexturePtr;
}
HeightMap* TextureLoader::CreateHeightMap(const File& filename, const vec2ui& requestedSize,
                                          const TextureParameters& params)
{
    // textures_.find(filename);
    auto ox   = requestedSize.x % 2 == 0 ? 1 : 0;
    auto oy   = requestedSize.y % 2 == 0 ? 1 : 0;
    auto size = vec2ui(ox, oy) + requestedSize;

    auto dataSize = size.x * size.y;
    std::vector<float> floatData;
    floatData.resize(dataSize);
    for (auto& f : floatData)
        f = 0.f;

    Utils::Image image;
    image.width  = size.x;
    image.height = size.y;
    image.setChannels(1);
    image.moveData(std::move(floatData));

    auto heightmapTexture    = std::make_unique<HeightMap>(graphicsApi_, params, filename, std::move(image));
    auto heightmapTexturePtr = heightmapTexture.get();

    SaveHeightMap(*heightmapTexture, filename);
    AddTexture(filename.GetAbsoultePath(), std::move(heightmapTexture), params.loadType);
    return heightmapTexturePtr;
}
GraphicsApi::IGraphicsApi& TextureLoader::GetGraphicsApi()
{
    return graphicsApi_;
}
void TextureLoader::DeleteTexture(Texture& texture)
{
    std::lock_guard<std::mutex> lk(textureMutex_);

    auto iter = std::find_if(textures_.begin(), textures_.end(), [id = texture.GetGpuObjectId()](const auto& texture) {
        return (texture.second.resource_->GetGpuObjectId() == id);
    });

    if (iter != textures_.end())
    {
        if (textureNotFound_.first and
            iter->second.resource_->GetGraphicsObjectId() == textureNotFound_.first->GetGraphicsObjectId())
        {
            return;
        }
        auto& textureInfo = iter->second;
        --textureInfo.instances_;

        if (textureInfo.instances_ > 0 or releaseLockState_)
            return;

        gpuResourceLoader_.AddObjectToRelease(std::move(textureInfo.resource_));
        textures_.erase(iter);
        // DEBUG_LOG("textures_ erase , size : " + std::to_string(textures_.size()));
    }
    else
    {
        ERROR_LOG("Texture not found. GpuObjectId=" + std::to_string(texture.GetGpuObjectId()));
    }
}

void TextureLoader::LockReleaseResources()
{
    releaseLockState_ = true;
}

void TextureLoader::UnlockReleaseResources()
{
    releaseLockState_ = false;
}

HeightMap* TextureLoader::LoadHeightMapBinary(const File& inputFileName, const TextureParameters& params)
{
    auto fp = fopen(inputFileName.GetAbsoultePath().c_str(), "rb");

    if (!fp)
    {
        ERROR_LOG("GetFileType: wrong image format or file does not exist : " + inputFileName.GetAbsoultePath());
        return nullptr;
    }

    HeightMapHeader header;
    auto bytes = fread(&header, sizeof(HeightMapHeader), 1, fp);

    if (bytes == 0)
    {
        ERROR_LOG("Read file error." + inputFileName.GetAbsoultePath());
    }

    DEBUG_LOG("Size : " + std::to_string(header.width) + "x" + std::to_string(header.height));
    DEBUG_LOG("Height map scale : " + std::to_string(header.scale));

    auto size = header.width * header.height;
    std::vector<float> floatData;
    floatData.resize(size);
    bytes = fread(&floatData[0], sizeof(float), size, fp);
    fclose(fp);

    if (bytes < size)
    {
        ERROR_LOG("Read file error." + inputFileName.GetAbsoultePath() + " " + ", bytes : " + std::to_string(bytes) +
                  "/" + std::to_string(sizeof(float) * size));
        return nullptr;
    }

    Utils::Image image;
    image.width  = header.width;
    image.height = header.height;
    image.setChannels(1);
    image.moveData(std::move(floatData));

    auto heightmapTexture    = std::make_unique<HeightMap>(graphicsApi_, params, inputFileName, std::move(image));
    heightmapTexture->SetScale(header.scale);
    auto heightmapTexturePtr = heightmapTexture.get();
    AddTexture(inputFileName.GetAbsoultePath(), std::move(heightmapTexture), params.loadType);
    return heightmapTexturePtr;
}
HeightMap* TextureLoader::LoadHeightMapTexture(const File& inputFileName, const TextureParameters& params)
{
    auto maybeImage = ReadFile(inputFileName, params);

    if (not maybeImage)
    {
        return nullptr;
    }

    auto& image              = *maybeImage;
    auto heightmapTexture    = std::make_unique<HeightMap>(graphicsApi_, params, inputFileName, std::move(image));
    auto heightmapTexturePtr = heightmapTexture.get();
    AddTexture(inputFileName.GetAbsoultePath(), std::move(heightmapTexture), params.loadType);
    return heightmapTexturePtr;
}
Texture* TextureLoader::GetTextureIfLoaded(const std::string& name, const TextureParameters& params)
{
    if (textures_.count(name))
    {
        //   DEBUG_LOG("Created texture already exist : " + name);

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

void convertImageToCpp(const Utils::Image& image)
{
    DEBUG_LOG("createCppFile...");
    std::ofstream ofs("/home/bach/Projects/OpenGL_Engine/Data/textureNotFound.h", std::ofstream::out);
    ofs << "#pragma once\n\n";
    //ofs << "#include <array>\n";
    ofs << "namespace GameEngine\n";
    ofs << "{\n";
    ofs << "namespace Resource\n";
    ofs << "{\n";
    ofs << "namespace TextureNotFound\n";
    ofs << "{\n";
    ofs << "static const unsigned int width{" << image.width << "};\n";
    ofs << "static const unsigned int height{" << image.height << "};\n";
    ofs << "static const unsigned int chanelsCount{" << static_cast<uint32>(image.getChannelsCount()) << "};\n";
    ofs << "static const unsigned char data[] = {";

    auto data = reinterpret_cast<const uint8*>(image.getRawDataPtr());
    uint32 count = image.width * image.height * image.getChannelsCount();
    for(uint32 i = 0; i < count; ++i)
    {
        unsigned char v = data[i];
        ofs << static_cast<uint32>(v);
        if (i < count - 1)
            ofs << ",";
    }
    ofs << "};\n";
    ofs << "} // namespace TextureNotFound\n";
    ofs << "} // namespace Resource\n";
    ofs << "} // namespace GameEngine\n";
    ofs.close();
    exit(0);

}
GeneralTexture* TextureLoader::GetTextureNotFound()
{
    const std::string textureName("cppTexture_textureNotFound");

    if (textureNotFound_.second)
    {
        auto texture = textures_.find(textureName);
        if (texture != textures_.end())
        {
            ++texture->second.instances_;
        }
        return textureNotFound_.first;
    }

    TextureParameters params;
    // params.flipMode = TextureFlip::VERTICAL;
    // auto image = ReadFile("textureNotFound.png", params);
    // convertImageToCpp(*image);

    textureNotFound_.second = true;
    Utils::Image textureNotFoundImage;
    textureNotFoundImage.width  = Resource::TextureNotFound::width;
    textureNotFoundImage.height = Resource::TextureNotFound::height;
    textureNotFoundImage.setChannels(Resource::TextureNotFound::chanelsCount);
    textureNotFoundImage.allocateImage<uint8>();
    textureNotFoundImage.copyImage<uint8>(Resource::TextureNotFound::data);

    auto texture = std::make_unique<GeneralTexture>(graphicsApi_, textureNotFoundImage, params);
    textureNotFound_.first = texture.get();
    AddTexture(textureName, std::move(texture), params.loadType);

    return textureNotFound_.first;
}
void TextureLoader::AddTexture(const std::string& name, std::unique_ptr<Texture> texture, TextureLoadType loadType)
{
    ResourceInfo<Texture> textureInfo;
    textureInfo.resourceGpuStatus_ = Convert(loadType);
    textureInfo.resource_          = std::move(texture);

    auto result = textureInfo.resource_.get();
    textures_.insert({name, std::move(textureInfo)});
    ApplyLoadTypeAction(*result, loadType);
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
