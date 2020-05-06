#include "ResourceUtils.h"

#include <FreeImage.h>
#include <GameEngine/Engine/Configuration.h>
#include <Logger/Log.h>
#include <Utils/Image/ImageUtils.h>

#include <filesystem>

#include "GLM/GLMUtils.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainHeightTools.h"
#include "HeightMapHeader.h"

namespace GameEngine
{
std::optional<Image> ReadFile(const InputFileName& inputFileName, const TextureParameters& params)
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

void CreateHeightMap(const std::string& in, const std::string& out, const vec3& scale)
{
    auto input  = EngineConf_GetFullDataPath(in);
    auto output = EngineConf_GetFullDataPath(out);

    auto fp = fopen(output.c_str(), "wb+");

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

    if (std::filesystem::exists(output))
    {
        std::filesystem::copy(output, output + ".backup");
    }

    auto fp = fopen(output.c_str(), "wb");

    if (not fp)
    {
        ERROR_LOG("cannot open file : " + output);
        return;
    }

    HeightMapHeader header;
    header.height = image.height;
    header.width  = image.width;
    header.scale  = vec3(1.f);

    auto size = header.width * header.height;
    fwrite(&header, sizeof(HeightMapHeader), 1, fp);
    fwrite(&image.floatData[0], sizeof(float), size, fp);
    fclose(fp);
}
void SetPixel(std::vector<uint8>& data, uint32 width, const vec2ui& position, const Color& color)
{
    auto startIndex = 4 * (position.x + position.y * width);
    if (startIndex + 3 < data.size())
    {
        data[startIndex]     = color.r();
        data[startIndex + 1] = color.g();
        data[startIndex + 2] = color.b();
        data[startIndex + 3] = color.a();
    }
}
float getPixel(const std::vector<float>& data, const vec2ui& size, const vec2ui& position)
{
    return data[position.x + position.y * size.x];
}
void GenerateBlendMap(const vec3& terrainScale, const HeightMap& heightMap, const OutputFileName& file, const vec2& thresholds)
{
    vec3 upVector(0.f, 1.f, 0.f);
    std::vector<uint8> imageData;
    std::vector<uint8> imageDataPreview;

    auto width = heightMap.GetImage().width;
    TerrainHeightTools tools(terrainScale, heightMap.GetImage().floatData, width, 0);

    for (uint32 j = 0; j < width; ++j)
    {
        for (uint32 i = 0; i < width; ++i)
        {
            auto normal = tools.GetNormal(i, j);
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
           
            imageData.push_back(0);
            imageData.push_back(static_cast<uint8>(value * 255.f));
            imageData.push_back(0);
            imageData.push_back(0);

            // create imageDataPreview with Alpha = 1
            imageDataPreview.push_back(0);
            imageDataPreview.push_back(static_cast<uint8>(value * 255.f));
            imageDataPreview.push_back(0);
            imageDataPreview.push_back(255);
        }
    }

    Utils::SaveImage(imageData, heightMap.GetSize(), file, vec2(4));
    Utils::SaveImage(imageDataPreview, heightMap.GetSize(), file + "_alpha1_preview");
    Utils::SaveImage(imageDataPreview, heightMap.GetSize(), file + "_alpha1_preview_scaled", vec2(4));
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
            // normal = glm::normalize(0.25f * vec3(2.f * (z4 - z3), 2.f * (z6 - z1), -4));
            // normal = glm::normalize(vec3((z4 - z3) ,2.f , (z6 - z1)));
            // bgr
            result.push_back(normal.z);
            result.push_back(normal.y);
            result.push_back(normal.x);
        }
    }

    return result;
}
}  // namespace GameEngine
