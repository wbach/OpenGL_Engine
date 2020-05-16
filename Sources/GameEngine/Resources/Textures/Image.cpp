#include "Image.h"

#include <Logger/Log.h>

namespace GameEngine
{
void Image::SetPixel(const vec2ui& position, const Color& color)
{
    auto startIndex = 4 * (position.x + position.y * width);

    if (not data.empty())
    {
        if (startIndex + 3 < data.size())
        {
            data[startIndex]     = color.r();
            data[startIndex + 1] = color.g();
            data[startIndex + 2] = color.b();
            data[startIndex + 3] = color.a();
        }
    }
    else if (not floatData.empty())
    {
        if (startIndex + 3 < floatData.size())
        {
            floatData[startIndex]     = color.r();
            floatData[startIndex + 1] = color.g();
            floatData[startIndex + 2] = color.b();
            floatData[startIndex + 3] = color.a();
        }
    }
}
vec2ui Image::Size() const
{
    return vec2ui(width, height);
}

std::optional<Color> Image::GetPixel(const vec2ui& position) const
{
    auto startIndex = 4 * (position.x + position.y * width);
    if (not data.empty())
    {
        if (startIndex + 3 < data.size())
        {
            return Color(data[startIndex], data[startIndex + 1], data[startIndex + 2], data[startIndex + 3]);
        }
    }
    else if (not floatData.empty())
    {
        if (startIndex + 3 < floatData.size())
        {
            return Color(floatData[startIndex], floatData[startIndex + 1], floatData[startIndex + 2],
                         floatData[startIndex + 3]);
        }
    }
    return std::nullopt;
}

}  // namespace GameEngine
