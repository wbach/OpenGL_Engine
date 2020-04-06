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
#include "Textures/MaterialTexture.h"
#include "Textures/NormalTexture.h"

namespace GameEngine
{
struct Header
{
    uint32 width;
    uint32 height;
    vec3 scale;
};
const std::string HEIGHTMAP_EXTENSION = "terrain";

TextureLoader::TextureLoader(GraphicsApi::IGraphicsApi& graphicsApi,
                             std::vector<std::unique_ptr<Texture>>& textures_vector,
                             std::shared_ptr<IGpuResourceLoader> gpuLoader)
    : graphicsApi_(graphicsApi)
    , textures_(textures_vector)
    , gpuResourceLoader_(gpuLoader)
    , textureNotFound_(nullptr)
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
        ERROR_LOG("GetFileType: wrong image format or file does not exist : " + file_location);
        return {};
    }

    FIBITMAP* imagen2 = FreeImage_Load(formato, file_location.c_str());
    if (!imagen2)
    {
        ERROR_LOG("FreeImageLoad: wrong image format or file does not exist : " + file_location);
        return {};
    }

    FIBITMAP* imagen = FreeImage_ConvertTo32Bits(imagen2);
    if (!imagen)
    {
        FreeImage_Unload(imagen2);
        ERROR_LOG("Cant convert to 32 bits : " + file_location);
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
            imagen = FreeImage_Rescale(imagen, static_cast<int>(w), static_cast<int>(h), FILTER_BSPLINE);
    }

    Image resultImage;
    resultImage.width  = w;
    resultImage.height = h;

    auto pixeles = FreeImage_GetBits(imagen);
    DEBUG_LOG("File convert bgr2rgb" + file_location + ".");

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

    DEBUG_LOG("File: " + file_location + " is loaded.");

    return std::move(resultImage);
}

Texture* TextureLoader::CreateTexture(const std::string& name, GraphicsApi::TextureType, GraphicsApi::TextureFilter,
                                      GraphicsApi::TextureMipmap, GraphicsApi::BufferAtachment, vec2ui size, void* data)
{
    DEBUG_LOG(name);
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
        return GetTextureNotFound();
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
        ERROR_LOG("Cube map texture need 6 texutres.");
        return nullptr;
    }

    std::vector<Image> images;
    images.resize(6);

    size_t x = 0;
    for (const auto& file : files)
    {
        auto image =
            ReadFile(EngineConf_GetFullDataPathAddToRequierd(file), applySizeLimit, TextureFlip::Type::VERTICAL);

        if (not image)
        {
            return GetTextureNotFound();
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

    auto isBinnary = Utils::CheckExtension(filename, HEIGHTMAP_EXTENSION);
    return isBinnary ? LoadHeightMapBinary(filename, gpu_pass) : LoadHeightMapTexture(filename, gpu_pass);
}

Texture* TextureLoader::LoadNormalMap(const std::vector<float>& baseData, const vec2ui& size, float strength)
{
    auto texture   = std::make_unique<Image>();
    auto& text     = *texture;
    text.width     = size.x;
    text.height    = size.y;
    text.floatData = createNromalMapData(size, baseData, strength);

    auto normaltexture = new NormalTexture(graphicsApi_, true, "noname_NormalTexutre", "nopath", std::move(texture));
    textures_.emplace_back(normaltexture);

    gpuResourceLoader_->AddObjectToGpuLoadingPass(normaltexture);

    return normaltexture;
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

    auto optImage = ReadFile(input, false);

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
Texture* TextureLoader::LoadHeightMapBinary(const std::string& filename, bool gpu_pass)
{
    auto fp = fopen(filename.c_str(), "rb");

    if (!fp)
    {
        ERROR_LOG("GetFileType: wrong image format or file does not exist : " + filename);
        return nullptr;
    }

    Header header;
    auto bytes = fread(&header, sizeof(Header), 1, fp);

    if (bytes == 0)
    {
        ERROR_LOG("Read file error." + filename);
    }

    DEBUG_LOG("Size : " + std::to_string(header.width) + "x" + std::to_string(header.height));
    DEBUG_LOG("Height map scale : " + std::to_string(header.scale));

    Image texture;
    texture.width = header.width;
    texture.height = header.height;

    auto size = header.width * header.height;
    texture.floatData.resize(size);

    bytes = fread(&texture.floatData[0], sizeof(float), size, fp);
    fclose(fp);

    if (bytes < size)
    {
        ERROR_LOG("Read file error." + filename + " " + ", bytes : " + std::to_string(bytes) + "/" +
                  std::to_string(sizeof(float) * size));
        return nullptr;
    }

    auto heightmapTexture = new HeightMap(graphicsApi_, true, filename, filename, std::move(texture));
    heightmapTexture->SetScale(header.scale);
    textures_.emplace_back(heightmapTexture);

    if (gpu_pass)
        gpuResourceLoader_->AddObjectToGpuLoadingPass(heightmapTexture);

    return heightmapTexture;
}
Texture* TextureLoader::LoadHeightMapTexture(const std::string& filename, bool gpu_pass)
{
    DEBUG_LOG(filename);
    auto optImage = ReadFile(filename, false);

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

    auto heightmap_texture = new HeightMap(graphicsApi_, true, filename, filename, std::move(image));
    heightmap_texture->SetScale(vec3(heightMapFactor_));
    textures_.emplace_back(heightmap_texture);

    if (gpu_pass)
        gpuResourceLoader_->AddObjectToGpuLoadingPass(heightmap_texture);

    return heightmap_texture;
}
Texture* TextureLoader::GetTextureIfLoaded(const std::string& filename) const
{
    auto iter = std::find_if(textures_.begin(), textures_.end(), [&filename](const std::unique_ptr<Texture>& texture) {
        return texture->GetFileName() == filename;
    });
    return iter != textures_.end() ? iter->get() : nullptr;
}

Texture* TextureLoader::GetTextureNotFound()
{
    if (not textureNotFound_)
    {
        textureNotFound_ = LoadTexture("Textures/textureNotFound.png", false);
    }

    return textureNotFound_;
}
}  // namespace GameEngine
