#include "ResourceUtils.h"

#include <FreeImage.h>
#include <GameEngine/Engine/Configuration.h>
#include <GraphicsApi/MeshRawData.h>
#include <Logger/Log.h>
#include <Utils/Image/ImageUtils.h>
#include <Utils/Variant.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <filesystem>
#include <optional>

#include "GLM/GLMUtils.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainHeightTools.h"
#include "HeightMapHeader.h"

namespace GameEngine
{
FIBITMAP* convertTo32bppIfDifferent(FIBITMAP* image)
{
    auto bpp = FreeImage_GetBPP(image);

    if (bpp != 32)
    {
        /* LOG TO FIX*/ LOG_ERROR << ("Convert image from " + std::to_string(bpp) + " bpp to 32 bpp");

        FIBITMAP* image32bit = FreeImage_ConvertTo32Bits(image);
        FreeImage_Unload(image);

        if (not image32bit)
        {
            return nullptr;
        }
        return image32bit;
    }
    return image;
}

FIBITMAP* resizeImageIfisLimited(FIBITMAP* image, SizeLimitPolicy sizeLimitPolicy)
{
    if (sizeLimitPolicy == SizeLimitPolicy::Limited and EngineConf.renderer.textures.limitTextureSize)
    {
        auto& textureSize{EngineConf.renderer.textures.maxSize};
        bool resize_texture{false};

        uint32 w = FreeImage_GetWidth(image);
        uint32 h = FreeImage_GetHeight(image);
        vec2ui newImageSize(w, h);

        if (w > textureSize->x)
        {
            newImageSize.x = textureSize->x;
            resize_texture = true;
        }
        if (h > textureSize->y)
        {
            newImageSize.y = textureSize->y;
            resize_texture = true;
        }

        if (resize_texture)
        {
            /* LOG TO FIX*/ LOG_ERROR << ("Resize image from " + std::to_string(vec2ui(w, h)) + " to " +
                                          std::to_string(newImageSize));

            auto resizedImage =
                FreeImage_Rescale(image, static_cast<int>(newImageSize.x), static_cast<int>(newImageSize.y), FILTER_BSPLINE);

            if (resizedImage)
            {
                FreeImage_Unload(image);
                return resizedImage;
            }
            else
            {
                /* LOG TO FIX*/ LOG_ERROR << ("Resize error.");
            }
        }
    }

    return image;
}

void flipImageIfRequest(FIBITMAP* image, TextureFlip flipMode)
{
    if (flipMode == TextureFlip::VERTICAL or flipMode == TextureFlip::BOTH)
    {
        FreeImage_FlipVertical(image);
    }
    if (flipMode == TextureFlip::HORIZONTAL or flipMode == TextureFlip::BOTH)
    {
        FreeImage_FlipHorizontal(image);
    }
}

std::optional<Utils::Image> ReadFile(const File& file, const TextureParameters& params)
{
    if (not file)
    {
        LOG_ERROR << "File not exist : " << file;
        return {};
    }

    FREE_IMAGE_FORMAT imageFormat = FreeImage_GetFileType(file.GetAbsolutePath().string().c_str(), 0);

    if (imageFormat == FIF_UNKNOWN)
    {
        LOG_ERROR << "GetFileType: wrong image format : " << file;
        return {};
    }

    FIBITMAP* image = FreeImage_Load(imageFormat, file.GetAbsolutePath().string().c_str());
    if (not image)
    {
        LOG_ERROR << "FreeImageLoad load failed: " << file;
        return {};
    }

    image = resizeImageIfisLimited(image, params.sizeLimitPolicy);
    flipImageIfRequest(image, params.flipMode);
    image = convertTo32bppIfDifferent(image);

    if (not image)
    {
        LOG_ERROR << "Cant convert to 32 bits: " << file;
        return {};
    }

    Utils::Image resultImage;
    resultImage.width  = FreeImage_GetWidth(image);
    resultImage.height = FreeImage_GetHeight(image);
    resultImage.setChannels(4);
    resultImage.allocateImage<uint8>();

    auto pixeles = FreeImage_GetBits(image);
    // bgr2rgb
    for (uint32 j = 0; j < resultImage.width * resultImage.height; j++)
    {
        resultImage.setPixel(vec2ui(j % resultImage.width, j / resultImage.width),
                             Color(pixeles[j * 4 + 2], pixeles[j * 4 + 1], pixeles[j * 4 + 0], pixeles[j * 4 + 3]));
    }
    FreeImage_Unload(image);
    /* LOG TO FIX*/ LOG_ERROR << ("File: " + file.GetBaseName() + " is loaded. Size: " + std::to_string(resultImage.width) + "x" +
                                  std::to_string(resultImage.height));
    return std::move(resultImage);
}

std::optional<Utils::Image> ReadImage(const unsigned char* data, unsigned int len, const TextureParameters& params)
{
    FIMEMORY* stream = FreeImage_OpenMemory(const_cast<BYTE*>(data), static_cast<DWORD>(len));
    if (not stream)
    {
        printf("Nie mozna otworzyc pamieci!\n");
        return std::nullopt;
    }

    FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromMemory(stream, 0);
    if (format == FIF_UNKNOWN)
    {
        FreeImage_CloseMemory(stream);
        return std::nullopt;
    }

    // Wczytujemy obraz z pamieci
    FIBITMAP* image = FreeImage_LoadFromMemory(format, stream, 0);
    if (not image)
    {
        printf("Nie mozna wczytac obrazu!\n");
        FreeImage_CloseMemory(stream);
        return std::nullopt;
    }

    image = resizeImageIfisLimited(image, params.sizeLimitPolicy);
    flipImageIfRequest(image, params.flipMode);
    image = convertTo32bppIfDifferent(image);

    if (not image)
    {
        /* LOG TO FIX*/ LOG_ERROR << ("Cant convert to 32 bits memory image.");
        return {};
    }

    Utils::Image resultImage;
    resultImage.width  = FreeImage_GetWidth(image);
    resultImage.height = FreeImage_GetHeight(image);
    resultImage.setChannels(4);
    resultImage.allocateImage<uint8>();

    auto pixeles = FreeImage_GetBits(image);
    // bgr2rgb
    for (uint32 j = 0; j < resultImage.width * resultImage.height; j++)
    {
        resultImage.setPixel(vec2ui(j % resultImage.width, j / resultImage.width),
                             Color(pixeles[j * 4 + 2], pixeles[j * 4 + 1], pixeles[j * 4 + 0], pixeles[j * 4 + 3]));
    }
    FreeImage_Unload(image);
    /* LOG TO FIX*/ LOG_ERROR << ("Image is loaded. Size: " + std::to_string(resultImage.width) + "x" +
                                  std::to_string(resultImage.height));
    return std::move(resultImage);
}

std::vector<float> ConvertHeightData(const std::vector<uint8>& data)
{
    std::vector<float> output;
    output.reserve(data.size());
    for (auto i = 0u; i < data.size(); i += 4)
    {
        auto color = (Utils::RGBtoFloat(data[i]) + Utils::RGBtoFloat(data[i + 1]) + Utils::RGBtoFloat(data[i + 2])) / 3.f;
        output.push_back(color);
    }
    return output;
}

void CreateHeightMap(const File& in, const File& out, const vec3& scale)
{
    auto fp = fopen(out.GetAbsolutePath().string().c_str(), "wb+");

    if (!fp)
    {
        LOG_ERROR << "cannot open file : " << out;
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

    std::visit(visitor{[&](const std::vector<uint8>& inputData)
                       {
                           auto data = ConvertHeightData(inputData);
                           fwrite(&data[0], sizeof(float), size, fp);
                       },
                       [&](const std::vector<float>& data) { fwrite(&data[0], sizeof(float), size, fp); },
                       [](std::monostate) { /* LOG TO FIX*/
                                            LOG_ERROR << ("Data not set!");
                       }},
               image.getImageData());
    fclose(fp);
}

void CreateHeightMap(const File& out, const vec2ui& size)
{
    HeightMapHeader header;
    header.height = size.x;
    header.width  = size.y;
    header.scale  = vec3(1);
    auto dataSize = header.width * header.height;

    auto fp = fopen(out.GetAbsolutePath().string().c_str(), "wb+");

    if (not fp)
    {
        LOG_ERROR << "cannot open file : " << out;
        return;
    }

    fwrite(&header, sizeof(HeightMapHeader), 1, fp);
    std::vector<float> floatData;
    floatData.resize(dataSize);
    memset(&floatData[0], 0, dataSize);
    fwrite(&floatData[0], sizeof(float), dataSize, fp);
    fclose(fp);
}

void SaveHeightMap(const HeightMap& heightmap, const File& outfile)
{
    auto& image = heightmap.GetImage();
    if (image.empty())
    {
        LOG_ERROR << "Can not save height map without data!";
        return;
    }

    Utils::CreateBackupFile(outfile.GetAbsolutePath().string());
    auto fp = fopen(outfile.GetAbsolutePath().string().c_str(), "wb");

    if (not fp)
    {
        LOG_ERROR << "cannot open file : " << outfile;
        return;
    }

    LOG_DEBUG << "outfile: " << outfile;

    HeightMapHeader header;
    header.height = image.height;
    header.width  = image.width;
    header.scale  = heightmap.GetScale();

    auto size = header.width * header.height;
    fwrite(&header, sizeof(HeightMapHeader), 1, fp);
    std::visit(visitor{[&](const std::vector<uint8>& inputData)
                       {
                           auto data = ConvertHeightData(inputData);
                           fwrite(&data[0], sizeof(float), size, fp);
                       },
                       [&](const std::vector<float>& data) { fwrite(&data[0], sizeof(float), size, fp); },
                       [](std::monostate) { LOG_ERROR << "Data not set!"; }},
               image.getImageData());
    fclose(fp);

    Utils::SaveImage(image, outfile.CreateFileWithExtension("png").GetAbsolutePath().string());
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

Utils::Image GenerateBlendMapImage(const vec3& terrainScale, const HeightMap& heightMap, const vec2& thresholds)
{
    auto width = heightMap.GetImage().width;
    TerrainHeightTools tools(terrainScale, heightMap.GetImage());
    Utils::Image outputImage;
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

    Utils::Image normalImage;
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

    std::visit(visitor{[&](std::vector<uint8> data)
                       {
                           Utils::SaveImage(data, vec3ui(heightMap.GetSize(), 4), file.GetAbsolutePath().string(), vec2(4));
                           for (size_t i = 3; i < data.size(); i += 4)
                               data[i] = 255;

                           Utils::SaveImage(data, vec3ui(image.size(), image.getChannelsCount()), file.GetAbsolutePath().string() + "_alpha1_preview");
                           Utils::SaveImage(data, vec3ui(image.size(), image.getChannelsCount()), file.GetAbsolutePath().string() + "_alpha1_preview_scaled",
                                            vec2(4));
                       },
                       [&](const std::vector<float>&) { LOG_WARN << ("GenerateBlendMapImage for floats not implemented"); },
                       [](std::monostate) { LOG_ERROR << ("Data not set!"); }},
               image.getImageData());
}

GameEngine::BoundingBox ComputeBoundingBox(const GraphicsApi::MeshRawData& mesh)
{
    if (mesh.positions_.empty())
    {
        LOG_DEBUG << "Positons empty return default";
        return GameEngine::BoundingBox();
    }

    vec3 minPos(mesh.positions_[0], mesh.positions_[1], mesh.positions_[2]);
    vec3 maxPos = minPos;

    for (size_t i = 3; i < mesh.positions_.size(); i += 3)
    {
        float x = mesh.positions_[i + 0];
        float y = mesh.positions_[i + 1];
        float z = mesh.positions_[i + 2];

        if (x < minPos.x)
            minPos.x = x;
        if (y < minPos.y)
            minPos.y = y;
        if (z < minPos.z)
            minPos.z = z;

        if (x > maxPos.x)
            maxPos.x = x;
        if (y > maxPos.y)
            maxPos.y = y;
        if (z > maxPos.z)
            maxPos.z = z;
    }

    return GameEngine::BoundingBox(minPos, maxPos);
}

}  // namespace GameEngine
