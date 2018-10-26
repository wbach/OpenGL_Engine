#pragma once
#include <unordered_map>
#include "Controller.h"
#include "Types.h"

namespace GameEngine
{
namespace Collada
{
struct LibraryControllers
{
    std::unordered_map<std::string, Controller> controllers_;
};
}
}
