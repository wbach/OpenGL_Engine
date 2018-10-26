#pragma once
#include <unordered_map>
#include "Image.h"
#include "Types.h"

namespace GameEngine
{
namespace Collada
{
struct LibraryImages
{
    std::unordered_map<std::string, Image> images_;
};
}
}
