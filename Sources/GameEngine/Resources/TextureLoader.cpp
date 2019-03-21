#define _CRT_SECURE_NO_WARNINGS
#include "TextureLoader.h"
#include <FreeImage.h>
#include <algorithm>
#include <fstream>
#include "../Engine/Configuration.h"
#include "GLM/GLMUtils.h"
#include "GpuResourceLoader.h"
#include "IGpuResourceLoader.h"
#include "Logger/Log.h"
#include "Textures/CubeMapTexture.h"
#include "Textures/GeneralTexture.h"
#include "Textures/HeightMap.h"
#include "Textures/NormalTexture.h"
#include "Textures/MaterialTexture.h"

namespace GameEngine
{
struct Header
{
    uint32 width;
    uint32 height;
};

TextureLoader::TextureLoader(GraphicsApi::IGraphicsApi& graphicsApi,
                             std::vector<std::unique_ptr<Texture>>& textures_vector,
                             std::shared_ptr<IGpuResourceLoader> gpuLoader)
    : graphicsApi_(graphicsApi)
    , textures_(textures_vector)
    , gpuResourceLoader_(gpuLoader)
    , heightMapFactor_(1.f)
{
}

TextureLoader::~TextureLoader()
{
}

std::optional<Image> TextureLoader::ReadFile(const std::string& file, bool applySizeLimit, TextureFlip::Type flip_mode)
{
    auto file_location        = file;
    FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(file_location.c_str(), 0);
    if (formato == FIF_UNKNOWN)
    {
        Error("GetFileType: wrong image format or file does not exist : " + file_location);
        return {};
    }

    FIBITMAP* imagen2 = FreeImage_Load(formato, file_location.c_str());
    if (!imagen2)
    {
        Error("FreeImageLoad: wrong image format or file does not exist : " + file_location);
        return {};
    }

    FIBITMAP* imagen = FreeImage_ConvertTo32Bits(imagen2);
    if (!imagen)
    {
        FreeImage_Unload(imagen2);
        Error("Cant convert to 32 bits : " + file_location);
        return {};
    }
    FreeImage_Unload(imagen2);

    if (flip_mode == TextureFlip::Type::VERTICAL || flip_mode == TextureFlip::Type::BOTH)
    {
        FreeImage_FlipVertical(imagen);
    }

    if (flip_mode == TextureFlip::Type::HORIZONTAL || flip_mode == TextureFlip::Type::BOTH)
    {
        FreeImage_FlipHorizontal(imagen);
    }

    uint32 w = FreeImage_GetWidth(imagen);
    uint32 h = FreeImage_GetHeight(imagen);

    if (applySizeLimit)
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
            imagen = FreeImage_Rescale(imagen, w, h, FILTER_BSPLINE);
    }

    Image resultImage;
    resultImage.width  = w;
    resultImage.height = h;

    char* pixeles = (char*)FreeImage_GetBits(imagen);
    Log("File convert bgr2rgb" + file_location + ".");

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

    Log("File: " + file_location + " is loaded.");

    return resultImage;
}

Texture* TextureLoader::CreateTexture(const std::string& name, GraphicsApi::TextureType type,
                                      GraphicsApi::TextureFilter filter, GraphicsApi::TextureMipmap mimpamp,
                                      GraphicsApi::BufferAtachment atachment, vec2ui size, void* data)
{
    textures_.emplace_back(new GeneralTexture(graphicsApi_, name, size, data));
    gpuResourceLoader_->AddObjectToGpuLoadingPass(textures_.back().get());
    return textures_.back().get();
}

Texture* TextureLoader::LoadTexture(const std::string& file, bool applySizeLimit, bool gpu_pass, ObjectTextureType type,
                                    TextureFlip::Type flip_mode)
{
    if (file.empty())
        return nullptr;

    if (auto texture = GetTextureIfLoaded(file))
    {
        return texture;
    }

    auto texture = ReadFile(EngineConf_GetFullDataPathAddToRequierd(file), applySizeLimit, flip_mode);

    if (not texture)
    {
        return nullptr;
    }

    switch (type)
    {
        case ObjectTextureType::MATERIAL:
        {
            textures_.emplace_back(new MaterialTexture(graphicsApi_, false, file, file, *texture));
        }
        break;
        case ObjectTextureType::HEIGHT_MAP:
            break;
    }
    if (gpu_pass)
    {
        gpuResourceLoader_->AddObjectToGpuLoadingPass(textures_.back().get());
    }

    return textures_.back().get();
}

Texture* TextureLoader::LoadTextureImmediately(const std::string& file, bool applySizeLimit, ObjectTextureType type,
                                               TextureFlip::Type flip_mode)
{
    if (file.empty())
        return nullptr;

    auto texture = LoadTexture(file, applySizeLimit, false, type, flip_mode);
    texture->GpuLoadingPass();
    return texture;
}

Texture* TextureLoader::LoadCubeMap(const std::vector<std::string>& files, bool applySizeLimit, bool gpu_pass)
{
    if (files.size() != 6)
    {
        Log("Cube map texture need 6 texutres.");
        return nullptr;
    }

    std::vector<Image> images;
    images.resize(6);

    int x = 0;
    for (const auto& file : files)
    {
        auto image =
            ReadFile(EngineConf_GetFullDataPathAddToRequierd(file), applySizeLimit, TextureFlip::Type::VERTICAL);

        if (not image)
        {
            return nullptr;
        }

        images[x++] = std::move(*image);
    }

    textures_.emplace_back(new CubeMapTexture(graphicsApi_, files[0], images));

    auto texture = textures_.back().get();

    if (gpu_pass)
        gpuResourceLoader_->AddObjectToGpuLoadingPass(texture);

    return texture;
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

Texture* TextureLoader::LoadHeightMap(const std::string& filename, bool gpu_pass)
{
    if (auto texture = GetTextureIfLoaded(filename))
    {
        return texture;
    }

    auto fp = fopen(filename.c_str(), "rb");

    if (!fp)
    {
        Error("GetFileType: wrong image format or file does not exist : " + filename);
        return nullptr;
    }

    Header header;
    auto bytes = fread(&header, sizeof(Header), 1, fp);

    if (bytes == 0)
    {
        Error("Read file error." + filename);
    }

    Log(" Size : " + std::to_string(header.width) + "x" + std::to_string(header.height));

    ImagePtr texture(new Image);
    auto& text  = *texture;
    text.width  = header.width;
    text.height = header.height;

    auto size = header.width * header.height;
    text.floatData.resize(size);

    bytes = fread(&text.floatData[0], sizeof(float), size, fp);

    if (bytes < sizeof(float) * size)
    {
        Error("Read file error." + filename + " " + ", bytes : " + std::to_string(bytes));
    }

    fclose(fp);

    for (auto& height : text.floatData)
    {
        height *= heightMapFactor_;
    }

    auto heightmap_texture = new HeightMap(graphicsApi_, true, filename, filename, texture);
    textures_.emplace_back(heightmap_texture);

    if (gpu_pass)
        gpuResourceLoader_->AddObjectToGpuLoadingPass(heightmap_texture);

    return heightmap_texture;
}

Texture* TextureLoader::LoadNormalMap(const std::vector<float>& baseData, const vec2ui& size, float strength)
{
    ImagePtr texture(new Image);
    auto& text = *texture;
    text.width = size.x;
    text.height = size.y;
    text.floatData = std::move(createNromalMapData(size, baseData, strength));

    auto normaltexture = new NormalTexture(graphicsApi_, true, "noname_NormalTexutre", "nopath", texture);
    textures_.emplace_back(normaltexture);

    gpuResourceLoader_->AddObjectToGpuLoadingPass(normaltexture);

    return normaltexture;
}

void TextureLoader::CreateHeightMap(const std::string& in, const std::string& out)
{
    auto input  = EngineConf_GetFullDataPath(in);
    auto output = EngineConf_GetFullDataPath(out);
    auto fp     = fopen(output.c_str(), "wb+");

    if (!fp)
    {
        Error("cannot open file : " + output);
        return;
    }

    auto optImage = ReadFile(input, false);

    if (not optImage)
    {
        return;
    }
    const auto& image = *optImage;

    Header header;
    header.height = image.height;
    header.width  = image.width;
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
void TextureLoader::SaveTextureToFile(const std::string& name, const std::vector<uint8>& data, const vec2ui& size, uint8 bytes, GraphicsApi::TextureFormat format) const
{
    auto bytesData = const_cast<uint8*>(&data[0]);
    auto im = FreeImage_ConvertFromRawBits(bytesData, size.x, size.y, bytes * size.x, 8 * bytes, 0, 0, 0);


    FREE_IMAGE_FORMAT fformat = FREE_IMAGE_FORMAT::FIF_PNG;
    std::string ext = ".png";

    if (format == GraphicsApi::TextureFormat::BMP)
    {
        fformat = FREE_IMAGE_FORMAT::FIF_BMP;
        ext = ".bmp";
    }

    if (format == GraphicsApi::TextureFormat::JPG)
    {
        fformat = FREE_IMAGE_FORMAT::FIF_JPEG;
        ext = ".jpeg";
    }

    FreeImage_Save(fformat, im, (name + ext).c_str(), 0);
}
Texture* TextureLoader::GetTextureIfLoaded(const std::string& filename) const
{
    auto iter = std::find_if(textures_.begin(), textures_.end(), [&filename](const std::unique_ptr<Texture>& texture) {
        return texture->GetFileName() == filename;
    });
    return iter != textures_.end() ? iter->get() : nullptr;
}
}  // namespace GameEngine
