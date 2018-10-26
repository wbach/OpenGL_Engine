#pragma once
#include <unordered_map>
#include "../Common/Source.h"
#include "Joints.h"
#include "Types.h"
#include "VertexWeights.h"

namespace GameEngine
{
namespace Collada
{
struct Skin
{
    Joitns joitns_;
    std::string source;
    mat4 bindShapeMatrix_;
    VertexWeights vertexWeights_;
    std::unordered_map<std::string, Source> sources_;
};
}
}
