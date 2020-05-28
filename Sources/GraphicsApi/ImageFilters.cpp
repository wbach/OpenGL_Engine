#include "ImageFilters.h"

namespace GraphicsApi
{
namespace
{
// clang-format off
const std::vector<float> gaussian3x3 = {
    0.077847f, 0.123317f, 0.077847f,
    0.123317f, 0.195346f, 0.123317f,
    0.077847f, 0.123317f, 0.077847f
};
const std::vector<float> gaussian5x5 = {
0.003765f, 0.015019f, 0.023792f, 0.015019f, 0.003765f,
0.015019f, 0.059912f, 0.094907f, 0.059912f, 0.015019f,
0.023792f, 0.094907f, 0.150342f, 0.094907f, 0.023792f,
0.015019f, 0.059912f, 0.094907f, 0.059912f, 0.015019f,
0.003765f, 0.015019f, 0.023792f, 0.015019f, 0.003765f
};
const std::vector<float> gaussian7x7 = {
0.000036f, 0.000363f, 0.001446f, 0.002291f, 0.001446f, 0.000363f, 0.000036f,
0.000363f, 0.003676f, 0.014662f, 0.023226f, 0.014662f, 0.003676f, 0.000363f,
0.001446f, 0.014662f, 0.058488f, 0.092651f, 0.058488f, 0.014662f, 0.001446f,
0.002291f, 0.023226f, 0.092651f, 0.146768f, 0.092651f, 0.023226f, 0.002291f,
0.001446f, 0.014662f, 0.058488f, 0.092651f, 0.058488f, 0.014662f, 0.001446f,
0.000363f, 0.003676f, 0.014662f, 0.023226f, 0.014662f, 0.003676f, 0.000363f,
0.000036f, 0.000363f, 0.001446f, 0.002291f, 0.001446f, 0.000363f, 0.000036f
};
// clang-format on
}  // namespace
ImageFilter gaussian3x3Filter()
{
    return ImageFilter{gaussian3x3, vec2ui(3, 3)};
}
ImageFilter gaussian5x5Filter()
{
    return ImageFilter{gaussian5x5, vec2ui(5, 5)};
}
ImageFilter gaussian7x7Filter()
{
    return ImageFilter{gaussian7x7, vec2ui(7, 7)};
}

std::optional<float> ImageFilter::getSample(const vec2ui& position) const
{
    return getSample(position.x, position.y);
}

std::optional<float> ImageFilter::getSample(uint32 x, uint32 y) const
{
    auto index = static_cast<size_t>(x + y * size.x);

    if (index < kernel.size())
        return kernel[index];
    return std::nullopt;
}

}  // namespace GraphicsApi
