#pragma once
#include "Skin.h"
#include "Types.h"

namespace GameEngine
{
namespace Collada
{
struct Controller
{
    std::string id_;
    std::string name_;
    std::unordered_map<std::string, Skin> skins_;
};
}
}
