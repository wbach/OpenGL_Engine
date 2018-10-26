#pragma once
#include "../Types.h"
#include "GameEngine/Resources/VboTypes.h"
#include <vector>

namespace Utils
{
class EnableVao
{
public:
    EnableVao(const uint32 vao, const GameEngine::VboMap& vertexArrays);
        EnableVao(const uint32 vao, const GameEngine::VboMap& vertexArrays, const std::vector<GameEngine::VertexBufferObjects>& ignored);
    ~EnableVao();

private:
    void Enable();
    void Disable();

    std::vector<uint32> v;
};
}
