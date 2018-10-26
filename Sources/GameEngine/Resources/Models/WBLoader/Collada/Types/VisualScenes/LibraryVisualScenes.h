#pragma once
#include <unordered_map>
#include "Types.h"
#include "VisualScene.h"

namespace GameEngine
{
namespace Collada
{
struct LibraryVisualScenes
{
    std::unordered_map<std::string, VisualScene> visualScenes_;
};
}
}
