#pragma once
#include "../Types.h"
#include <vector>

namespace Utils
{
class EnableVao
{
public:
    EnableVao(const uint32 vao, const std::vector<uint8>& vertexArrays);
    ~EnableVao();

private:
    void Enable();
    void Disable();

    std::vector<uint8> v;
};
}
