#pragma once
#include <algorithm>
#include <optional>
#include <vector>

#include "Image.h"
#include "Types.h"

namespace Utils
{
template <typename T>
void ConvertImageData(const std::vector<T>& inputData, std::vector<uint8>& outputData, size_t channels, T modifier)
{
    if (channels > 4)
        return;

    if (channels == 4)
    {
        std::transform(inputData.begin(), inputData.end(), outputData.begin(),
                       [m = modifier](auto v) { return static_cast<uint8>(m * v); });
        return;
    }

    size_t i = 0;
    for (size_t fi = 0; fi < inputData.size(); fi += channels)
    {
        uint8 r{255}, g{255}, b{255}, a{255};

        r = static_cast<uint8>(modifier * inputData[fi]);
        if (channels > 1)
        {
            g = static_cast<uint8>(modifier * inputData[fi + 1]);
            if (channels > 2)
            {
                b = static_cast<uint8>(modifier * inputData[fi + 2]);
                if (channels > 3)
                {
                    a = static_cast<uint8>(modifier * inputData[fi + 3]);
                }
            }
        }
        outputData[i]     = r;
        outputData[i + 1] = g;
        outputData[i + 2] = b;
        outputData[i + 3] = a;
    }
}

void SaveImage(const std::vector<uint8>&, const vec2ui&, const std::string&, const std::optional<vec2>& = std::nullopt);
void SaveImage(const std::vector<float>&, const vec3ui&, const std::string&, const std::optional<vec2>&);
void SaveImage(const Image&, const std::string&, const std::optional<vec2>& = std::nullopt);

template <class T>
Image cloneImageAs(const Image&);

Image cloneImageAsFloat(const Image&);
Image cloneImageAsUint8(const Image&);
}  // namespace Utils
