#include "ImageUtils.h"

#include <FreeImage.h>
#include <Logger/Log.h>
#include <Utils/Variant.h>

#include <algorithm>
#include <filesystem>

namespace Utils
{
void SaveImage(const std::vector<uint8> &data, const vec2ui &size, const std::string &filename,
               const std::optional<vec2> &scale)
{
    auto minSize = size.x * size.y * 4;
    if (data.size() < minSize)
    {
        DEBUG_LOG("Data image not complete : " + filename + " size : " + std::to_string(data.size()) + "/" +
                  std::to_string(minSize));
        return;
    }

    DEBUG_LOG("Save image : " + filename + ", size=" + std::to_string(size.x) + "x" + std::to_string(size.y) + ":4");
    FIBITMAP *bitmap = FreeImage_Allocate(static_cast<int>(size.x), static_cast<int>(size.y), 32, 8, 8, 8);

    auto width  = FreeImage_GetWidth(bitmap);
    auto height = FreeImage_GetHeight(bitmap);

    size_t dataIndex = 0;
    for (uint32 y = 0; y < height; ++y)
    {
        BYTE *bits = FreeImage_GetScanLine(bitmap, static_cast<int>(y));
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
        auto scaledBitmap =
            FreeImage_Rescale(bitmap, size.x * scale->x, size.y * scale->y, FREE_IMAGE_FILTER::FILTER_BICUBIC);

        FreeImage_Unload(bitmap);
        bitmap = scaledBitmap;
    }
    auto outputFilename = std::filesystem::path(filename).replace_extension(".png").string();
    FreeImage_FlipVertical(bitmap);
    FreeImage_Save(FIF_PNG, bitmap, outputFilename.c_str());
    FreeImage_Unload(bitmap);
}

void SaveImage(const Image &image, const std::string &outputFilePath, const std::optional<vec2> &scale)
{
    std::visit(
        visitor{[&](std::vector<uint8> data) { SaveImage(data, image.size(), outputFilePath, scale); },
                [&](const std::vector<float> &) { DEBUG_LOG("SaveImage for floats not implemented"); },
                [](std::monostate) { ERROR_LOG("Data not set!"); }},
        image.getImageData());
}

}  // namespace Utils
