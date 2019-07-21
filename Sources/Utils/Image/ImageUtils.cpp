#include "ImageUtils.h"
#include <FreeImage.h>
#include <Logger/Log.h>

namespace Utils
{
void SaveImage(const std::vector<uint8> &data, const vec2ui &size, const std::string &filename)
{
    auto minSize = size.x *size.y * 4;
    if (data.size() < minSize)
    {
        DEBUG_LOG("Data image not complete : " + filename + " size : " + std::to_string(data.size()) + "/" + std::to_string(minSize));
        return;
    }
    DEBUG_LOG("Save image : " + filename);
    FIBITMAP *bitmap = FreeImage_Allocate(size.x, size.y, 32, 8, 8, 8);

    auto width  = FreeImage_GetWidth(bitmap);
    auto height = FreeImage_GetHeight(bitmap);

    size_t dataIndex = 0;
    for (int y = 0; y < height; ++y)
    {
        BYTE *bits = FreeImage_GetScanLine(bitmap, y);
        for (int x = 0; x < width; x++)
        {
            bits[2] = data[dataIndex++];  // blue
            bits[1] = data[dataIndex++];  // green
            bits[0] = data[dataIndex++];  // red
            bits[3] = data[dataIndex++];
            bits += 4;
        }
    }
    FreeImage_FlipVertical(bitmap);
    FreeImage_Save(FIF_PNG, bitmap, (filename + ".png").c_str(), PNG_DEFAULT);
    FreeImage_Unload(bitmap);
}
}  // namespace Utils
