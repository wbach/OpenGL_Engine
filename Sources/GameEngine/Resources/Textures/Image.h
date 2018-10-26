#pragma once
#include <memory>
#include <vector>
#include "Types.h"

namespace GameEngine
{
struct Image
{
    std::vector<uint8> data;
    std::vector<float> floatData;

    uint32 width  = 0;
    uint32 height = 0;
};

typedef std::shared_ptr<Image> ImagePtr;
}  // namespace GameEngine
