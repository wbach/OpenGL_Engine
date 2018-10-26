#pragma once
#include <vector>
#include "GameEngine/Resources/Models/WBLoader/Collada/Types/Common/Input.h"
#include "Types.h"

namespace GameEngine
{
namespace Collada
{
struct AnimationSampler
{
    std::string id_;
    std::vector<Input> inputs_;
};
}
}
