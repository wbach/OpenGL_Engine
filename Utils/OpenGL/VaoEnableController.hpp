#pragma once
#include "../Types.h"
#include <vector>

namespace Utils
{
class EnableVao
{
public:
    EnableVao(uint32 vao, const std::vector<uint32>& vertexArrays);
    ~EnableVao();

private:
    void Enable();
    void Disable();

    std::vector<uint32> v;
};
}
