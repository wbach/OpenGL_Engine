#include "ImageUtils.h"

#include <FreeImage.h>
#include <Logger/Log.h>
#include <Utils/Variant.h>

#include <filesystem>
#include <variant>

namespace Utils
{
void SaveImage(const std::vector<uint8>& data, const vec2ui& size, const std::string& filename, const std::optional<vec2>& scale)
{
    auto minSize = size.x * size.y * 4;
    if (data.size() < minSize)
    {
        LOG_ERROR << "Data image not complete : " << filename << " size : " << data.size() << "/" << std::to_string(minSize);
        return;
    }

    LOG_DEBUG << "Save image : " << filename << ", size=" << size.x << "x" << size.y << ":4";
    FIBITMAP* bitmap = FreeImage_Allocate(static_cast<int>(size.x), static_cast<int>(size.y), 32, 8, 8, 8);

    auto width  = FreeImage_GetWidth(bitmap);
    auto height = FreeImage_GetHeight(bitmap);

    size_t dataIndex = 0;
    for (uint32 y = 0; y < height; ++y)
    {
        BYTE* bits = FreeImage_GetScanLine(bitmap, static_cast<int>(y));
        for (uint32 x = 0; x < width; x++)
        {
            bits[2] = data[dataIndex++];  // red
            bits[1] = data[dataIndex++];  // green
            bits[0] = data[dataIndex++];  // blue
            bits[3] = data[dataIndex++];
            bits += 4;
        }
    }
    if (scale)
    {
        auto scaledBitmap = FreeImage_Rescale(bitmap, static_cast<int>(size.x * scale->x), static_cast<int>(size.y * scale->y),
                                              FREE_IMAGE_FILTER::FILTER_BICUBIC);

        FreeImage_Unload(bitmap);
        bitmap = scaledBitmap;
    }
    auto outputFilename = std::filesystem::path(filename).replace_extension(".png").string();
    FreeImage_FlipVertical(bitmap);
    FreeImage_Save(FIF_PNG, bitmap, outputFilename.c_str());
    FreeImage_Unload(bitmap);
}

void SaveImage32FEXR(const std::vector<float>& data, const vec3ui& size, const std::string& filename)
{
    FreeImage_Initialise();

    FIBITMAP* bitmap = nullptr;

    if (size.z == 1)
        bitmap = FreeImage_AllocateT(FIT_FLOAT, size.x, size.y, 32);
    else if (size.z == 3)
        bitmap = FreeImage_AllocateT(FIT_RGBF, size.x, size.y, 0, 0, 0, 0);
    else if (size.z == 4)
        bitmap = FreeImage_AllocateT(FIT_RGBAF, size.x, size.y, 0, 0, 0, 0);
    else
    {
        LOG_ERROR << "Nieobslugiwana liczba kanalow: " << size.z;
        FreeImage_DeInitialise();
        return;
    }

    for (unsigned y = 0; y < size.y; ++y)
    {
        if (size.z == 1)
        {
            float* scanline = (float*)FreeImage_GetScanLine(bitmap, y);
            for (unsigned x = 0; x < size.x; ++x)
                scanline[x] = data[y * size.x + x];
        }
        else if (size.z == 3)
        {
            FIRGBF* scanline = (FIRGBF*)FreeImage_GetScanLine(bitmap, y);
            for (unsigned x = 0; x < size.x; ++x)
            {
                const float* src  = &data[(y * size.x + x) * 3];
                scanline[x].red   = src[0];
                scanline[x].green = src[1];
                scanline[x].blue  = src[2];
            }
        }
        else if (size.z == 4)
        {
            FIRGBAF* scanline = (FIRGBAF*)FreeImage_GetScanLine(bitmap, y);
            for (unsigned x = 0; x < size.x; ++x)
            {
                const float* src  = &data[(y * size.x + x) * 4];
                scanline[x].red   = src[0];
                scanline[x].green = src[1];
                scanline[x].blue  = src[2];
                scanline[x].alpha = src[3];
            }
        }
    }

    FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(filename.c_str());
    if (!FreeImage_Save(fif, bitmap, filename.c_str()))
        LOG_ERROR << "Nie udalo sie zapisac EXR 32-bit: " << filename;
    else
        LOG_DEBUG << "EXR 32-bit zapisany pomyslnie: " << filename;

    FreeImage_Unload(bitmap);
    FreeImage_DeInitialise();
}

void SaveImage(const std::vector<float>& data, const vec3ui& size, const std::string& filename, const std::optional<vec2>& scale)
{
    // Wyciagniecie rozszerzenia
    std::string ext = filename.substr(filename.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "exr")
    {
        // EXR -> uzyj funkcji 32-bit float
        SaveImage32FEXR(data, size, filename);
    }
    else if (ext == "hdr" || ext == "tiff")
    {
        // Pozostale formaty floatowe -> standard FreeImage
        FreeImage_Initialise();

        FIBITMAP* bitmap = nullptr;
        if (size.z == 1)
            bitmap = FreeImage_AllocateT(FIT_FLOAT, size.x, size.y, 32);
        else if (size.z == 3)
            bitmap = FreeImage_AllocateT(FIT_RGBF, size.x, size.y, 0, 0, 0, 0);
        else if (size.z == 4)
            bitmap = FreeImage_AllocateT(FIT_RGBAF, size.x, size.y, 0, 0, 0, 0);
        else
        {
            LOG_ERROR << "Unsupported channel size " << size.z;
            FreeImage_DeInitialise();
            return;
        }

        for (unsigned y = 0; y < size.y; ++y)
        {
            if (size.z == 1)
            {
                float* scanline = (float*)FreeImage_GetScanLine(bitmap, y);
                for (unsigned x = 0; x < size.x; ++x)
                    scanline[x] = data[y * size.x + x];
            }
            else if (size.z == 3)
            {
                FIRGBF* scanline = (FIRGBF*)FreeImage_GetScanLine(bitmap, y);
                for (unsigned x = 0; x < size.x; ++x)
                {
                    const float* src  = &data[(y * size.x + x) * 3];
                    scanline[x].red   = src[0];
                    scanline[x].green = src[1];
                    scanline[x].blue  = src[2];
                }
            }
            else if (size.z == 4)
            {
                FIRGBAF* scanline = (FIRGBAF*)FreeImage_GetScanLine(bitmap, y);
                for (unsigned x = 0; x < size.x; ++x)
                {
                    const float* src  = &data[(y * size.x + x) * 4];
                    scanline[x].red   = src[0];
                    scanline[x].green = src[1];
                    scanline[x].blue  = src[2];
                    scanline[x].alpha = src[3];
                }
            }
        }

        FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(filename.c_str());
        if (!FreeImage_Save(fif, bitmap, filename.c_str()))
            LOG_ERROR << "Save failed: " << filename;
        else
            LOG_DEBUG << "Float image saved: " << filename;

        FreeImage_Unload(bitmap);
        FreeImage_DeInitialise();
    }
    else
    {
        LOG_WARN << "Format " << ext << " nie wspiera floatow -- konwersja do 8-bit do zaimplementowania.";
    }
}

void SaveImage(const Image& image, const std::string& outputFilePath, const std::optional<vec2>& scale)
{
    LOG_DEBUG << "Try save to file image. outputFilePath = " << outputFilePath << ", size = " << scale;

    std::visit(visitor{[&](std::vector<uint8> data) { SaveImage(data, image.size(), outputFilePath, scale); },
                       [&](const std::vector<float>& data)
                       {
                           std::string ext = outputFilePath.substr(outputFilePath.find_last_of('.') + 1);
                           std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                           if (image.getChannelsCount() == 1 and ext == "png")
                           {
                               LOG_DEBUG << "SaveImage for floats, converting to uint8 image";

                               float min = data.front();
                               float max = data.front();
                               for (auto value : data)
                               {
                                   if (value < min)
                                       min = value;

                                   if (value > max)
                                       max = value;
                               }

                               std::vector<uint8> udata;
                               udata.reserve(data.size() * 4);

                               for (auto value : data)
                               {
                                   auto normalized = static_cast<uint8>((value - min) / (max - min) * 255);
                                   udata.push_back(normalized);
                                   udata.push_back(normalized);
                                   udata.push_back(normalized);
                                   udata.push_back(255);
                               }

                               SaveImage(udata, image.size(), outputFilePath, scale);
                           }
                           else
                           {
                               SaveImage(data, vec3ui(image.size().x, image.size().y, image.getChannelsCount()), outputFilePath,
                                         scale);
                           }
                       },
                       [](std::monostate) { LOG_ERROR << "Data not set!"; }},
               image.getImageData());
}

template <class T>
Image cloneImageAs(const Image& src)
{
    Image dst;
    dst.width  = src.width;
    dst.height = src.height;
    dst.setChannels(src.getChannelsCount());

    const auto& data = src.getImageData();

    // uint8 → T
    if (std::holds_alternative<std::vector<uint8>>(data))
    {
        const auto& s = std::get<std::vector<uint8>>(data);
        std::vector<T> d;
        d.resize(s.size());

        if constexpr (std::is_floating_point_v<T>)
        {
            for (size_t i = 0; i < s.size(); ++i)
                d[i] = static_cast<T>(s[i]) / static_cast<T>(255.0);
        }
        else
        {
            for (size_t i = 0; i < s.size(); ++i)
                d[i] = static_cast<T>(s[i]);
        }

        dst.moveData(std::move(d));
    }
    // float → T
    else if (std::holds_alternative<std::vector<float>>(data))
    {
        const auto& s = std::get<std::vector<float>>(data);
        std::vector<T> d;
        d.resize(s.size());

        if constexpr (std::is_same_v<T, uint8>)
        {
            for (size_t i = 0; i < s.size(); ++i)
                d[i] = static_cast<uint8>(glm::clamp(s[i], 0.0f, 1.0f) * 255.0f);
        }
        else
        {
            for (size_t i = 0; i < s.size(); ++i)
                d[i] = static_cast<T>(s[i]);
        }

        dst.moveData(std::move(d));
    }
    else
    {
        dst.allocateImage<T>();
    }

    return dst;
}
Image cloneImageAsFloat(const Image& src)
{
    return cloneImageAs<float>(src);
}
Image cloneImageAsUint8(const Image& src)
{
    return cloneImageAs<uint8>(src);
}

void ClearChannel(Image& image, uint8 channelToClear)
{
    if (channelToClear >= image.getChannelsCount())
        return;

    auto& dataVariant   = image.getImageData();
    const auto channels = image.getChannelsCount();

    std::visit(visitor{[&](auto& data)
                       {
                           using T = typename std::decay_t<decltype(data)>::value_type;

                           for (size_t i = channelToClear; i < data.size(); i += channels)
                           {
                               data[i] = static_cast<T>(0);
                           }
                       },

                       [](std::monostate) {}},
               dataVariant);
}
}  // namespace Utils
