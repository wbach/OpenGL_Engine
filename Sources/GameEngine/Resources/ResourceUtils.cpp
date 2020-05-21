#include "ResourceUtils.h"

#include <FreeImage.h>
#include <GameEngine/Engine/Configuration.h>
#include <Logger/Log.h>
#include <Utils/Image/ImageUtils.h>
#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <filesystem>

#include "GLM/GLMUtils.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainHeightTools.h"
#include "HeightMapHeader.h"

namespace GameEngine
{
std::optional<Image> ReadFile(const File& inputFileName, const TextureParameters& params)
{
    auto absoultePath = inputFileName.GetAbsoultePath();
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
    DEBUG_LOG("File: " + inputFileName.GetBaseName() + " is loaded.");
    return std::move(resultImage);
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
    textureParams.applySizeLimit = false;

    auto optImage = ReadFile(in, textureParams);

    if (not optImage)
    {
        return;
    }
    const auto& image = *optImage;

    HeightMapHeader header;
    header.height = image.height;
    header.width  = image.width;
    header.scale  = scale;
    auto size     = header.width * header.height;
    fwrite(&header, sizeof(HeightMapHeader), 1, fp);

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

void SaveHeightMap(const HeightMap& heightmap, const std::string& outfile)
{
    auto& image = heightmap.GetImage();
    if (image.floatData.empty())
    {
        ERROR_LOG("Can not save height map without data!");
        return;
    }

    auto output = EngineConf_GetFullDataPath(outfile);

    Utils::CreateBackupFile(output);
    auto fp = fopen(output.c_str(), "wb");

    if (not fp)
    {
        ERROR_LOG("cannot open file : " + output);
        return;
    }

    DEBUG_LOG(outfile);

    HeightMapHeader header;
    header.height = image.height;
    header.width  = image.width;
    header.scale  = vec3(1.f);

    auto size = header.width * header.height;
    fwrite(&header, sizeof(HeightMapHeader), 1, fp);
    fwrite(&image.floatData[0], sizeof(float), size, fp);
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

Image GenerateBlendMapImage(const vec3& terrainScale, const HeightMap& heightMap, const vec2& thresholds)
{
    auto width = heightMap.GetImage().width;
    TerrainHeightTools tools(terrainScale, heightMap.GetImage().floatData, width);

    Image outputImage;
    auto& imageData    = outputImage.data;
    outputImage.width  = heightMap.GetImage().width;
    outputImage.height = heightMap.GetImage().height;

    for (uint32 j = 0; j < width; ++j)
    {
        for (uint32 i = 0; i < width; ++i)
        {
            auto normal = tools.GetNormal(i, j);

            imageData.push_back(0);
            imageData.push_back(GetBlendValue(normal, thresholds));
            imageData.push_back(0);
            imageData.push_back(0);
        }
    }

    return outputImage;
}
std::unique_ptr<NormalTexture> CreateNormalTexture(GraphicsApi::IGraphicsApi& graphicsApi, const vec3& terrainScale,
                                                   const HeightMap& heightMap)
{
    auto width = heightMap.GetImage().width;
    TerrainHeightTools tools(terrainScale, heightMap.GetImage().floatData, width);

    Image normalImage;
    auto& imageData    = normalImage.floatData;
    normalImage.width  = heightMap.GetImage().width;
    normalImage.height = heightMap.GetImage().height;
    imageData.reserve(normalImage.width * normalImage.height * 3);

    for (uint32 j = 0; j < width; ++j)
    {
        for (uint32 i = 0; i < width; ++i)
        {
            auto normal = tools.GetNormal(i, j);
            imageData.push_back(normal.x);
            imageData.push_back(normal.y);
            imageData.push_back(normal.z);
        }
    }
    return std::make_unique<NormalTexture>(graphicsApi, std::move(normalImage));
}
float getPixel(const std::vector<float>& data, const vec2ui& size, const vec2ui& position)
{
    return data[position.x + position.y * size.x];
}
void GenerateBlendMap(const vec3& terrainScale, const HeightMap& heightMap, const File& file,
                      const vec2& thresholds)
{
    auto image = GenerateBlendMapImage(terrainScale, heightMap, thresholds);
    Utils::SaveImage(image.data, heightMap.GetSize(), file.GetAbsoultePath(), vec2(4));

    for (size_t i = 3; i < image.data.size(); i += 4)
        image.data[i] = 255;

    Utils::SaveImage(image.data, image.Size(), file.GetAbsoultePath() + "_alpha1_preview");
    Utils::SaveImage(image.data, image.Size(), file.GetAbsoultePath() + "_alpha1_preview_scaled", vec2(4));
}

Image CreateZerosImage(const vec2ui& size)
{
    Image image;
    image.width  = size.x;
    image.height = size.y;
    image.data.resize(size.x * size.y * 4);
    for (uint32 j = 0; j < size.y; ++j)
    {
        for (uint32 i = 0; i < size.x; ++i)
        {
            image.SetPixel(vec2ui(i, j), Color(0.f));
        }
    }
    return image;
}

}  // namespace GameEngine
