#pragma once
#include <unordered_map>
#include "Effect.h"

namespace GameEngine
{
namespace Collada
{
struct LibraryEffects
{
    std::unordered_map<std::string, Effect> effects_;
};
}
}
