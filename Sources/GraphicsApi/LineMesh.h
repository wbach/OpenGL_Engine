#pragma once
#include <vector>

namespace GraphicsApi
{
struct LineMesh
{
    std::vector<float> positions_;
    std::vector<float> colors_;
    bool updateBufferNeeded = true;
};
}  // namespace GraphicsApi
