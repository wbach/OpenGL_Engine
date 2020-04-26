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

namespace GameEngine
{
namespace
{
uint32 unknownTextureNameId{0};
}
struct Header
{
    uint32 width;
    uint32 height;
    vec3 scale;
};
const std::string HEIGHTMAP_EXTENSION = "terrain";

TextureLoader::TextureLoader(GraphicsApi::IGraphicsApi& graphicsApi, IGpuResourceLoader& gpuLoader,
                             std::unordered_map<std::string, ResourceInfo<Texture>>& textures)
    : graphicsApi_(graphicsApi)
    , gpuResourceLoader_(gpuLoader)
    , textures_(textures)
    , textureNotFound_({nullptr, false})
    , heightMapFactor_(1.f)
{
}

TextureLoader::~TextureLoader()
{
}

std::optional<Image> TextureLoader::ReadFile(const InputFileName& inputFileName, const TextureParameters& params)
{
    FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(inputFileName.c_str(), 0);
    if (formato == FIF_UNKNOWN)
    {
        ERROR_LOG("GetFileType: wrong image format or file does not exist : " + inputFileName);
        return {};
    }

    FIBITMAP* imagen2 = FreeImage_Load(formato, inputFileName.c_str());
    if (!imagen2)
    {
        ERROR_LOG("FreeImageLoad: wrong image format or file does not exist : " + inputFileName);
        return {};
    }

    FIBITMAP* imagen = FreeImage_ConvertTo32Bits(imagen2);
    if (!imagen)
    {
        FreeImage_Unload(imagen2);
        ERROR_LOG("Cant convert to 32 bits : " + inputFileName);
        return {};
    }
    FreeImage_Unload(imagen2);

    if (params.flipMode == TextureFlip::VERTICAL or params.flipMode == TextureFlip::BOTH)
    {
        FreeImage_FlipVertical(imagen);
    }

    if (params.flipMode == TextureFlip::HORIZONTAL or params.flipMode == TextureFlip::BOTH)
    {
        FreeImage_FlipHorizontal(imagen);
    }

    uint32 w = FreeImage_GetWidth(imagen);
    uint32 h = FreeImage_GetHeight(imagen);

    if (params.applySizeLimit)
    {
        auto& texture_size  = EngineConf.renderer.textures.maxSize;
        bool resize_texture = false;

        if (w > texture_size.x)
        {
            w              = texture_size.x;
            resize_texture = true;
        }
        if (h > texture_size.y)
        {
            h              = texture_size.y;
            resize_texture = true;
        }
        if (resize_texture)
            imagen = FreeImage_Rescale(imagen, static_cast<int>(w), static_cast<int>(h), FILTER_BSPLINE);
    }

    Image resultImage;
    resultImage.width  = w;
    resultImage.height = h;

    auto pixeles = FreeImage_GetBits(imagen);
    // DEBUG_LOG("File convert bgr2rgb" + inputFileName + ".");

    resultImage.data.resize(4 * w * h);

    // bgr2rgb
    for (uint32 j = 0; j < w * h; j++)
    {
        resultImage.data[j * 4 + 0] = pixeles[j * 4 + 2];
        resultImage.data[j * 4 + 1] = pixeles[j * 4 + 1];
        resultImage.data[j * 4 + 2] = pixeles[j * 4 + 0];
        resultImage.data[j * 4 + 3] = pixeles[j * 4 + 3];
    }

    FreeImage_Unload(imagen);
    DEBUG_LOG("File: " + inputFileName + " is loaded.");
    return std::move(resultImage);
}

Texture* TextureLoader::CreateTexture(const std::string& name, const TextureParameters& params, const TextureSize& size,
                                      RawData data)
{
    std::lock_guard<std::mutex> lk(textureMutex_);

    if (auto texture = GetTextureIfLoaded(name))
        return texture;

    return AddTexture(name, std::make_unique<GeneralTexture>(graphicsApi_, name, size, data), params.loadType);
}

Texture* TextureLoader::LoadTexture(const InputFileName& inputFileName, const TextureParameters& params)
{
    std::lock_guard<std::mutex> lk(textureMutex_);

    if (inputFileName.empty())
        return nullptr;

    if (auto texture = GetTextureIfLoaded(inputFileName))
        return texture;

    auto image = ReadFile(EngineConf_GetFullDataPathAddToRequierd(inputFileName), params);

    if (not image)
        return GetTextureNotFound();

    auto materialTexture = std::make_unique<MaterialTexture>(graphicsApi_, false, Utils::GetFilename(inputFileName),
                                                             Utils::GetFilePath(inputFileName), *image);

    return AddTexture(inputFileName, std::move(materialTexture), params.loadType);
}

Texture* TextureLoader::LoadCubeMap(const std::array<InputFileName, 6>& files, const TextureParameters& params)
{
    std::lock_guard<std::mutex> lk(textureMutex_);

    std::ostringstream textureName;
    for (const auto& file : files)
        textureName << file;

    if (auto texture = GetTextureIfLoaded(textureName.str()))
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

    return AddTexture(textureName.str(), std::make_unique<CubeMapTexture>(graphicsApi_, textureName.str(), images),
                      params.loadType);
}

float getPixel(const std::vector<float>& data, const vec2ui& size, const vec2ui& position)
{
    return data[position.x + position.y * size.x];
}

std::vector<float> createNromalMapData(const vec2ui& size, const std::vector<float>& heightMapData,
                                       float normalStrength)
{
    // z0 -- z1 -- z2
    // |	 |     |
    // z3 -- h  -- z4
    // |     |     |
    // z5 -- z6 -- z7

    std::vector<float> result;

    for (uint32 j = 0; j < size.y; ++j)
    {
        for (uint32 i = 0; i < size.x; ++i)
        {
            uint32 x = i;
            uint32 y = j;

            if (i == 0)
                x = i + 1;

            if (i >= size.x - 1)
                x = i - 1;

            if (j == 0)
                y = j + 1;

            if (j >= size.y - 1)
                y = j - 1;

            float z0 = getPixel(heightMapData, size, vec2ui(x - 1, y - 1));
            float z1 = getPixel(heightMapData, size, vec2ui(x, y - 1));
            float z2 = getPixel(heightMapData, size, vec2ui(x + 1, y - 1));
            float z3 = getPixel(heightMapData, size, vec2ui(x - 1, y));
            float z4 = getPixel(heightMapData, size, vec2ui(x + 1, y));
            float z5 = getPixel(heightMapData, size, vec2ui(x - 1, y + 1));
            float z6 = getPixel(heightMapData, size, vec2ui(x, y + 1));
            float z7 = getPixel(heightMapData, size, vec2ui(x + 1, y + 1));

            vec3 normal;

            // Sobel Filter
            normal.z = 1.0f / normalStrength;
            normal.x = z0 + 2.f * z3 + z5 - z2 - 2.f * z4 - z7;
            normal.y = z0 + 2.f * z1 + z2 - z5 - 2.f * z6 - z7;

            normal = (glm::normalize(normal) + 1.0f) / 2.0f;

            // bgr
            result.push_back(normal.z);
            result.push_back(normal.y);
            result.push_back(normal.x);
        }
    }

    return result;
}

Texture* TextureLoader::LoadHeightMap(const InputFileName& inputFileName, const TextureParameters& params)
{
    std::lock_guard<std::mutex> lk(textureMutex_);

    if (auto texture = GetTextureIfLoaded(inputFileName))
    {
        return texture;
    }

    auto isBinnary = Utils::CheckExtension(inputFileName, HEIGHTMAP_EXTENSION);
    return isBinnary ? LoadHeightMapBinary(inputFileName, params) : LoadHeightMapTexture(inputFileName, params);
}

Texture* TextureLoader::LoadNormalMap(const std::vector<float>& baseData, const TextureSize& size, float strength)
{
    std::lock_guard<std::mutex> lk(textureMutex_);

    auto image       = std::make_unique<Image>();
    image->width     = size.x;
    image->height    = size.y;
    image->floatData = createNromalMapData(size, baseData, strength);
    auto normaltexture =
        std::make_unique<NormalTexture>(graphicsApi_, true, "noname_NormalTexutre", "nopath", std::move(image));

    return AddTexture(GetNoName(), std::move(normaltexture), TextureLoadType::AddToGpuPass);
}

void TextureLoader::CreateHeightMap(const std::string& in, const std::string& out, const vec3& scale)
{
    auto input  = EngineConf_GetFullDataPath(in);
    auto output = EngineConf_GetFullDataPath(out);
    auto fp     = fopen(output.c_str(), "wb+");

    if (!fp)
    {
        ERROR_LOG("cannot open file : " + output);
        return;
    }

    TextureParameters textureParams;
    textureParams.applySizeLimit = false;

    auto optImage = ReadFile(input, textureParams);

    if (not optImage)
    {
        return;
    }
    const auto& image = *optImage;

    Header header;
    header.height = image.height;
    header.width  = image.width;
    header.scale  = scale;
    auto size     = header.width * header.height;
    fwrite(&header, sizeof(Header), 1, fp);

    if (!image.floatData.empty())
    {
        fwrite(&image.floatData[0], sizeof(float), size, fp);
    }
    else
    {
        std::vector<float> data;
        data.reserve(size);
        for (auto i = 0u; i < image.data.size(); i += 4)
        {
            auto color = (Utils::RGBtoFloat(image.data[i]) + Utils::RGBtoFloat(image.data[i + 1]) +
                          Utils::RGBtoFloat(image.data[i + 2])) /
                         3.f;
            data.push_back(color);
        }
        fwrite(&data[0], sizeof(float), size, fp);
    }
    fclose(fp);
}

void TextureLoader::SetHeightMapFactor(float factor)
{
    heightMapFactor_ = factor;
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

    auto iter =
        std::find_if(textures_.begin(), textures_.end(), [id = texture.GetGpuObjectId()](const auto& texture) {
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

    Header header;
    auto bytes = fread(&header, sizeof(Header), 1, fp);

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

    auto heightmapTexture = std::make_unique<HeightMap>(graphicsApi_, true, Utils::GetFilename(inputFileName),
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

    auto heightmapTexture = std::make_unique<HeightMap>(graphicsApi_, true, Utils::GetFilename(inputFileName),
                                                        GetRelativeDataPath(inputFileName), std::move(image));
    heightmapTexture->SetScale(vec3(heightMapFactor_));

    return AddTexture(inputFileName, std::move(heightmapTexture), params.loadType);
}

Texture* TextureLoader::GetTextureIfLoaded(const std::string& name)
{
    if (textures_.count(name))
    {
        DEBUG_LOG("Created texture already exist : " + name);

        auto& textureInfo = textures_.at(name);
        ++textureInfo.instances_;
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
    textureInfo.resource_ = std::move(texture);
    auto result           = textureInfo.resource_.get();
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
}  // namespace GameEngine
