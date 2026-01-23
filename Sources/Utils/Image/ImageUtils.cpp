#include "ImageUtils.h"

#include <FreeImage.h>
#include <Logger/Log.h>
#include <Utils/Variant.h>

#include <cstddef>
#include <execution>
#include <filesystem>
#include <thread>

namespace Utils
{
void SaveImage(const std::vector<uint8>& data, const vec3ui& size, const std::filesystem::path& filename,
               const std::optional<vec2>& scale)
{
    // auto channels = size.z;
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

void SaveImage(const std::vector<float>& data, const vec3ui& size, const std::filesystem::path& filename,
               const std::optional<vec2>& scale)
{
    std::string ext = filename.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "exr")
    {
        SaveImage32FEXR(data, size, filename.string());
    }
    else if (ext == "hdr" || ext == "tiff")
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

        FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(filename.string().c_str());
        if (!FreeImage_Save(fif, bitmap, filename.string().c_str()))
            LOG_ERROR << "Save failed: " << filename;
        else
            LOG_DEBUG << "Float image saved: " << filename;

        FreeImage_Unload(bitmap);
        FreeImage_DeInitialise();
    }
    else
    {
        FreeImage_Initialise();

        FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(filename.string().c_str());
        if (fif == FIF_UNKNOWN)
        {
            LOG_ERROR << "Nieznany format pliku: " << filename;
            FreeImage_DeInitialise();
            return;
        }

        FIBITMAP* bitmap = nullptr;

        if (size.z == 1)
            bitmap = FreeImage_Allocate(size.x, size.y, 8);
        else if (size.z == 3)
            bitmap = FreeImage_Allocate(size.x, size.y, 24);
        else if (size.z == 4)
            bitmap = FreeImage_Allocate(size.x, size.y, 32);
        else
        {
            LOG_ERROR << "Nieobsługiwana liczba kanałów dla 8-bit: " << size.z;
            FreeImage_DeInitialise();
            return;
        }

        for (unsigned y = 0; y < size.y; ++y)
        {
            BYTE* scanline = FreeImage_GetScanLine(bitmap, y);
            for (unsigned x = 0; x < size.x; ++x)
            {
                for (unsigned c = 0; c < size.z; ++c)
                {
                    float val = data[(y * size.x + x) * size.z + c];

                    if (scale.has_value())
                    {
                        val = (val - scale->x) / (scale->y - scale->x);
                    }

                    BYTE byteVal = (BYTE)std::clamp(val * 255.0f + 0.5f, 0.0f, 255.0f);

                    if (size.z >= 3)
                    {
                        if (c == 0)
                            scanline[x * size.z + FI_RGBA_RED] = byteVal;
                        else if (c == 1)
                            scanline[x * size.z + FI_RGBA_GREEN] = byteVal;
                        else if (c == 2)
                            scanline[x * size.z + FI_RGBA_BLUE] = byteVal;
                        else if (c == 3)
                            scanline[x * size.z + FI_RGBA_ALPHA] = byteVal;
                    }
                    else
                    {
                        scanline[x] = byteVal;
                    }
                }
            }
        }

        if (!FreeImage_Save(fif, bitmap, filename.string().c_str()))
            LOG_ERROR << "Save failed (8-bit): " << filename;
        else
            LOG_DEBUG << "8-bit image saved: " << filename;

        FreeImage_Unload(bitmap);
        FreeImage_DeInitialise();
    }
}

void SaveImage(const Image& image, const std::filesystem::path& outputFilePath, const std::optional<vec2>& scale)
{
    LOG_DEBUG << "Try save to file image. outputFilePath = " << outputFilePath << ", size = " << scale;

    std::visit(visitor{[&](std::vector<uint8> data)
                       { SaveImage(data, vec3ui(image.size(), image.getChannelsCount()), outputFilePath, scale); },
                       [&](const std::vector<float>& data)
                       {
                           std::string ext = outputFilePath.extension().string();
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

                               SaveImage(udata, vec3ui(image.size(), image.getChannelsCount()), outputFilePath, scale);
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

void FastCopyPixels(const Image& srcImg, Image& dstImg, const std::vector<vec2ui>& points, size_t multiCoreThreshold)
{
    if (std::holds_alternative<std::vector<float>>(srcImg.getImageData()) &&
        std::holds_alternative<std::vector<uint8>>(dstImg.getImageData()))
    {
        const auto& srcData  = std::get<std::vector<float>>(srcImg.getImageData());
        auto& dstData        = std::get<std::vector<uint8>>(dstImg.getImageData());
        const uint8 channels = std::min(srcImg.getChannelsCount(), dstImg.getChannelsCount());
        const uint32 width   = srcImg.width;

        auto convertPixel = [&](const vec2ui& p)
        {
            size_t idx = (p.x + p.y * width) * channels;
            if (idx + channels > srcData.size() || idx + channels > dstData.size())
                return;

            for (uint8 c = 0; c < channels; ++c)
            {
                float val        = srcData[idx + c];
                val              = std::clamp(val, 0.0f, 1.0f);
                dstData[idx + c] = static_cast<uint8>(val * 255.0f);
            }
        };
        std::for_each(points.begin(), points.end(), convertPixel);
        // std::for_each(std::execution::par, points.begin(), points.end(), convertPixel);
    }
    else
    {
        for (const auto& p : points)
        {
            if (auto tmpColor = srcImg.getPixel(p))
                dstImg.setPixel(p, *tmpColor);
        }
    }
}

void LogImageData(const Utils::ImageData& data)
{
    std::visit(
        [](auto&& arg)
        {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::monostate>)
            {
                LOG_DEBUG << "ImageData jest puste (monostate)";
            }
            else if constexpr (std::is_same_v<T, std::vector<uint8_t>>)
            {
                LOG_DEBUG << "ImageData zawiera vector<uint8_t>, rozmiar = " << arg.size();
            }
            else if constexpr (std::is_same_v<T, std::vector<float>>)
            {
                LOG_DEBUG << "ImageData zawiera vector<float>, rozmiar = " << arg.size();
            }
        },
        data);
}
std::vector<uint8_t> compressWithFreeImagePNG(const std::vector<uint8_t>& rawRGBA, int width, int height)
{
    if (rawRGBA.size() != static_cast<size_t>(width * height * 4))
        throw std::runtime_error("Invalid buffer size");

    LOG_DEBUG << "Comporesing image. Raw data size: " << rawRGBA.size();

    FIBITMAP* bitmap = FreeImage_ConvertFromRawBits(const_cast<BYTE*>(rawRGBA.data()), width, height, width * 4, 32,
                                                    0xFF000000,  // R
                                                    0x00FF0000,  // G
                                                    0x0000FF00,  // B
                                                    true         // top-down (OpenGL style)
    );

    if (!bitmap)
        throw std::runtime_error("FreeImage bitmap creation failed");

    FIMEMORY* mem = FreeImage_OpenMemory();
    if (!mem)
    {
        FreeImage_Unload(bitmap);
        throw std::runtime_error("FreeImage_OpenMemory failed");
    }

    FreeImage_SaveToMemory(FIF_PNG, bitmap, mem, PNG_Z_BEST_COMPRESSION);

    BYTE* pngData = nullptr;
    DWORD pngSize = 0;
    FreeImage_AcquireMemory(mem, &pngData, &pngSize);

    std::vector<uint8_t> compressed(pngData, pngData + pngSize);

    FreeImage_CloseMemory(mem);
    FreeImage_Unload(bitmap);

    float reduction = 100.f - (static_cast<float>(compressed.size()) / rawRGBA.size() * 100.f);
    LOG_DEBUG << "Image compressed. New size: " << compressed.size() << " (reduction: " << std::fixed << std::setprecision(2)
              << reduction << "%)";
    return compressed;
}
std::vector<uint8_t> decompressWithFreeImagePNG(const std::vector<uint8_t>& compressedPNG, int& outWidth, int& outHeight)
{
    FIMEMORY* mem = FreeImage_OpenMemory(const_cast<BYTE*>(compressedPNG.data()), compressedPNG.size());

    if (!mem)
        throw std::runtime_error("FreeImage_OpenMemory failed");

    FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(mem);
    if (fif == FIF_UNKNOWN)
    {
        FreeImage_CloseMemory(mem);
        throw std::runtime_error("Unknown image format");
    }

    FIBITMAP* bitmap = FreeImage_LoadFromMemory(fif, mem);
    if (!bitmap)
    {
        FreeImage_CloseMemory(mem);
        throw std::runtime_error("FreeImage_LoadFromMemory failed");
    }

    outWidth  = FreeImage_GetWidth(bitmap);
    outHeight = FreeImage_GetHeight(bitmap);

    FIBITMAP* converted = FreeImage_ConvertTo32Bits(bitmap);
    FreeImage_Unload(bitmap);

    std::vector<uint8_t> raw(outWidth * outHeight * 4);

    FreeImage_ConvertToRawBits(raw.data(), converted, outWidth * 4, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, true);

    FreeImage_Unload(converted);
    FreeImage_CloseMemory(mem);

    float ratio = static_cast<float>(compressedPNG.size()) / static_cast<float>(raw.size()) * 100.f;

    LOG_DEBUG << "Image decompressed. New size: " << compressedPNG.size() << " (Ratio of original size: " << std::fixed
              << std::setprecision(2) << ratio << "%)";

    return raw;
}
void CompressImagesParallel(std::vector<Image>& images)
{
    if (images.empty())
    {
        LOG_DEBUG << "Empty images vector";
        return;
    }
    const size_t numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    size_t n         = images.size();
    size_t chunkSize = (n + numThreads - 1) / numThreads;

    for (size_t t = 0; t < numThreads; ++t)
    {
        size_t start = t * chunkSize;
        size_t end   = std::min(start + chunkSize, n);

        if (start >= end)
            break;

        threads.emplace_back(
            [&images, start, end]
            {
                for (size_t i = start; i < end; ++i)
                {
                    if (not images[i].isCompressed)
                        images[i].compressData();
                }
            });
    }

    for (auto& th : threads)
        th.join();
}
void DecompressImagesParallel(std::vector<Image>& images)
{
    if (images.empty())
    {
        LOG_DEBUG << "Empty images vector";
        return;
    }

    const size_t numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    size_t n         = images.size();
    size_t chunkSize = (n + numThreads - 1) / numThreads;

    for (size_t t = 0; t < numThreads; ++t)
    {
        size_t start = t * chunkSize;
        size_t end   = std::min(start + chunkSize, n);

        if (start >= end)
            break;

        threads.emplace_back(
            [&images, start, end]
            {
                for (size_t i = start; i < end; ++i)
                {
                    if (images[i].isCompressed)
                        images[i].decompressData();
                }
            });
    }

    for (auto& th : threads)
        th.join();
}
}  // namespace Utils
