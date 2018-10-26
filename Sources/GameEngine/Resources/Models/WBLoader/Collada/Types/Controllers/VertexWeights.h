#pragma once
#include <vector>
#include "GameEngine/Resources/Models/WBLoader/Collada/Types/Common/Input.h"
#include "Types.h"

namespace GameEngine
{
namespace Collada
{
struct VertexWeights
{
    uint32 count;
    std::vector<Input> inputs_;
    std::vector<uint32> vcount_;
    std::vector<uint32> v_;
};
}
}
