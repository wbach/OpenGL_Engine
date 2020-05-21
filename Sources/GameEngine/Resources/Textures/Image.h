#pragma once
#include <optional>
#include <vector>

#include "Types.h"

namespace GameEngine
{
struct Image
{
    std::vector<uint8> data;
    std::vector<float> floatData;

    uint32 width{0};
    uint32 height{0};

    vec2ui Size() const;

    const void* GetRawDataPtr() const
    {
        if (not data.empty())
            return &data[0];
        if (not floatData.empty())
            return &floatData[0];
        return nullptr;
    }

    std::optional<Color> GetPixel(const vec2ui&) const;
    bool SetPixel(const vec2ui&, const Color&);
};
}  // namespace GameEngine
