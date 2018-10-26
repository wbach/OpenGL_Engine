#pragma once
#include "Mesh.h"
#include "Types.h"

namespace GameEngine
{
namespace Collada
{
struct Geometry
{
    std::string id_;
    std::string name_;
    std::list<Mesh> meshes_;
};
}
}
