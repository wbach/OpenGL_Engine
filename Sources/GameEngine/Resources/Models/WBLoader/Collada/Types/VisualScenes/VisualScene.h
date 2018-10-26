#pragma once
#include <unordered_map>
#include "Node.h"
#include "Types.h"

namespace GameEngine
{
namespace Collada
{
struct VisualScene
{
    std::string id_;
    std::string name_;
    std::unordered_map<std::string, Node> nodes_;
};
}
}
