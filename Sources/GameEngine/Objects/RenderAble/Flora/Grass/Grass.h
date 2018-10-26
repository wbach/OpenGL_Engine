#pragma once
#include <vector>
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
class Model;

struct Grass : public GameObject
{
    Model *model;
    std::vector<float> grassvertex;
};
}  // GameEngine
