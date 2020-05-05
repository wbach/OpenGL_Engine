#include "Image.h"

namespace GameEngine
{
void SetPixel(Image& image, const vec2ui& position, const Color& color)
{
    auto startIndex = 4 * (position.x + position.y * image.width);
    if (startIndex + 3 < image.data.size())
    {
        image.data[startIndex]     = color.r();
        image.data[startIndex + 1] = color.g();
        image.data[startIndex + 2] = color.b();
        image.data[startIndex + 3] = color.a();
    }
}
vec2ui Image::Size() const
{
    return vec2ui(width, height);
}

std::optional<Color> GetPixel(Image& image, const vec2ui& position)
{
    auto startIndex = 4 * (position.x + position.y * image.width);
    if (startIndex + 3 < image.data.size())
    {
        return Color(image.data[startIndex], image.data[startIndex + 1], image.data[startIndex + 2],
                     image.data[startIndex + 3]);
    }
    return std::nullopt;
}

}  // namespace GameEngine
