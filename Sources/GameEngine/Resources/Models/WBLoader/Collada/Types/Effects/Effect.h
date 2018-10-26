#pragma once
#include <string>
#include <unordered_map>
#include "Color.h"
#include "Float.h"
#include "NewParam.h"

namespace GameEngine
{
namespace Collada
{
struct Effect
{
    std::string id;
    Color emission;
    Color ambient;
    Color diffuse;
    Color specular;
    Float shininess;
    Float indexOfRefraction;
    std::unordered_map<std::string, NewParam> newParams_;
};
}
}
