#pragma once
#include "../Types.h"
#include "GameEngine/Resources/VboTypes.h"
#include <vector>

namespace Utils
{
class EnableVao
{
public:
    EnableVao(const uint32 vao, const VboMap& vertexArrays);
	EnableVao(const uint32 vao, const VboMap& vertexArrays, const std::vector<VertexBufferObjects>& ignored);
    ~EnableVao();

private:
    void Enable();
    void Disable();

    std::vector<uint32> v;
};
}
