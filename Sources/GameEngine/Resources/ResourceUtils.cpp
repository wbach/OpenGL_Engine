#include "ResourceUtils.h"

#include <FreeImage.h>
#include <GameEngine/Engine/Configuration.h>
#include <Logger/Log.h>
#include <Utils/Image/ImageUtils.h>
#include <Utils/Variant.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <filesystem>

#include "GLM/GLMUtils.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainHeightTools.h"
#include "HeightMapHeader.h"

namespace GameEngine
{
std::optional<GraphicsApi::Image> ReadFile(const File& inputFileName, const TextureParameters& params)
{
    auto absoultePath         = inputFileName.GetAbsoultePath();
    FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(absoultePath.c_str(), 0);

    if (not std::filesystem::exists(absoultePath))
    {
        ERROR_LOG("File not exist : " + absoultePath);
        return {};
    }
    if (formato == FIF_UNKNOWN)
    {
        ERROR_LOG("GetFileType: wrong image format : " + absoultePath);
        return {};
    }

    FIBITMAP* imagen2 = FreeImage_Load(formato, absoultePath.c_str());
    if (!imagen2)
    {
        ERROR_LOG("FreeImageLoad load failed  : " + absoultePath);
        return {};
    }

    FIBITMAP* imagen = FreeImage_ConvertTo32Bits(imagen2);
    if (!imagen)
    {
        FreeImage_Unload(imagen2);
        ERROR_LOG("Cant convert to 32 bits : " + inputFileName.GetDataRelativeDir());
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

    if (params.sizeLimitPolicy == SizeLimitPolicy::Limited)
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

    GraphicsApi::Image resultImage;
    resultImage.width  = w;
    resultImage.height = h;
    resultImage.setChannels(4);
    resultImage.allocateImage<uint8>();

    auto pixeles = FreeImage_GetBits(imagen);
    // bgr2rgb
    for (uint32 j = 0; j < w * h; j++)
    {
        resultImage.setPixel(vec2ui(j % resultImage.width, j / resultImage.width),
                             Color(pixeles[j * 4 + 2], pixeles[j * 4 + 1], pixeles[j * 4 + 0], pixeles[j * 4 + 3]));
    }
    FreeImage_Unload(imagen);
    DEBUG_LOG("File: " + inputFileName.GetBaseName() + " is loaded.");
    return std::move(resultImage);
}

std::vector<float> ConvertHeightData(const std::vector<uint8>& data)
{
    std::vector<float> output;
    output.reserve(data.size());
    for (auto i = 0u; i < data.size(); i += 4)
    {
        auto color =
            (Utils::RGBtoFloat(data[i]) + Utils::RGBtoFloat(data[i + 1]) + Utils::RGBtoFloat(data[i + 2])) / 3.f;
        output.push_back(color);
    }
    return output;
}

void CreateHeightMap(const File& in, const File& out, const vec3& scale)
{
    auto fp = fopen(out.GetAbsoultePath().c_str(), "wb+");

    if (!fp)
    {
        ERROR_LOG("cannot open file : " + out.GetAbsoultePath());
        return;
    }

    TextureParameters textureParams;
    textureParams.sizeLimitPolicy = SizeLimitPolicy::NoLimited;
    auto maybeImage               = ReadFile(in, textureParams);
    if (not maybeImage)
    {
        return;
    }
    const auto& image = *maybeImage;

    HeightMapHeader header;
    header.height = image.height;
    header.width  = image.width;
    header.scale  = scale;
    auto size     = header.width * header.height;
    fwrite(&header, sizeof(HeightMapHeader), 1, fp);

    std::visit(visitor{[&](const std::vector<uint8>& inputData) {
                           auto data = ConvertHeightData(inputData);
                           fwrite(&data[0], sizeof(float), size, fp);
                       },
                       [&](const std::vector<float>& data) { fwrite(&data[0], sizeof(float), size, fp); },
                       [](std::monostate) { ERROR_LOG("Data not set!"); }},
               image.getImageData());
    fclose(fp);
}

void SaveHeightMap(const HeightMap& heightmap, const File& outfile)
{
    auto& image = heightmap.GetImage();
    if (image.empty())
    {
        ERROR_LOG("Can not save height map without data!");
        return;
    }

    Utils::CreateBackupFile(outfile.GetAbsoultePath());
    auto fp = fopen(outfile.GetAbsoultePath().c_str(), "wb");

    if (not fp)
    {
        ERROR_LOG("cannot open file : " + outfile.GetAbsoultePath());
        return;
    }

    DEBUG_LOG(outfile.GetAbsoultePath());

    HeightMapHeader header;
    header.height = image.height;
    header.width  = image.width;
    header.scale  = vec3(1.f);

    auto size = header.width * header.height;
    fwrite(&header, sizeof(HeightMapHeader), 1, fp);
    std::visit(visitor{[&](const std::vector<uint8>& inputData) {
                           auto data = ConvertHeightData(inputData);
                           fwrite(&data[0], sizeof(float), size, fp);
                       },
                       [&](const std::vector<float>& data) { fwrite(&data[0], sizeof(float), size, fp); },
                       [](std::monostate) { ERROR_LOG("Data not set!"); }},
               image.getImageData());
    fclose(fp);
}

uint8 GetBlendValue(const vec3& normal, const vec2& thresholds)
{
    float value = 1.f - normal.y;

    if (value > thresholds.x)
    {
        value = 1.f;
    }
    else if (value > thresholds.y)
    {
        value = (value - thresholds.y) / (thresholds.x - thresholds.y);
    }
    else
    {
        value = 0.f;
    }

    return static_cast<uint8>(value * 255.f);
}

GraphicsApi::Image GenerateBlendMapImage(const vec3& terrainScale, const HeightMap& heightMap, const vec2& thresholds)
{
    auto width = heightMap.GetImage().width;
    TerrainHeightTools tools(terrainScale, heightMap.GetImage());
    GraphicsApi::Image outputImage;
    outputImage.width  = heightMap.GetImage().width;
    outputImage.height = heightMap.GetImage().height;
    outputImage.setChannels(4);
    outputImage.allocateImage<uint8>();

    for (uint32 j = 0; j < width; ++j)
    {
        for (uint32 i = 0; i < width; ++i)
        {
            auto normal = tools.GetNormal(i, j);
            outputImage.setPixel({i, j}, Color(0, GetBlendValue(normal, thresholds), 0, 0));
        }
    }

    return outputImage;
}
std::unique_ptr<GeneralTexture> CreateNormalTexture(GraphicsApi::IGraphicsApi& graphicsApi, const vec3& terrainScale,
                                                    const HeightMap& heightMap)
{
    TerrainHeightTools tools(terrainScale, heightMap.GetImage());

    auto width = heightMap.GetImage().width;

    GraphicsApi::Image normalImage;
    normalImage.width  = heightMap.GetImage().width;
    normalImage.height = heightMap.GetImage().height;
    normalImage.setChannels(3);
    normalImage.allocateImage<float>();

    for (uint32 j = 0; j < width; ++j)
    {
        for (uint32 i = 0; i < width; ++i)
        {
            auto normal = tools.GetNormal(i, j);
            normalImage.setPixel({i, j}, Color(normal.x, normal.y, normal.z, 0.f));
        }
    }
    TextureParameters paramters;
    paramters.filter = GraphicsApi::TextureFilter::NEAREST;
    paramters.mimap  = GraphicsApi::TextureMipmap::NONE;
    return std::make_unique<GeneralTexture>(graphicsApi, std::move(normalImage), paramters, std::nullopt);
}
float getPixel(const std::vector<float>& data, const vec2ui& size, const vec2ui& position)
{
    return data[position.x + position.y * size.x];
}
void GenerateBlendMap(const vec3& terrainScale, const HeightMap& heightMap, const File& file, const vec2& thresholds)
{
    auto image = GenerateBlendMapImage(terrainScale, heightMap, thresholds);

    std::visit(
        visitor{[&](std::vector<uint8> data) {
                    Utils::SaveImage(data, heightMap.GetSize(), file.GetAbsoultePath(), vec2(4));
                    for (size_t i = 3; i < data.size(); i += 4)
                        data[i] = 255;

                    Utils::SaveImage(data, image.size(), file.GetAbsoultePath() + "_alpha1_preview");
                    Utils::SaveImage(data, image.size(), file.GetAbsoultePath() + "_alpha1_preview_scaled", vec2(4));
                },
                [&](const std::vector<float>& data) { DEBUG_LOG("GenerateBlendMapImage for floats not implemented"); },
                [](std::monostate) { ERROR_LOG("Data not set!"); }},
        image.getImageData());
}
}  // namespace GameEngine
