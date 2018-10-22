#pragma once
#include <vector>
#include "GameEngine/Objects/GameObject.h"

class CModel;

namespace GameEngine
{
struct Grass : public GameObject
{
    CModel *model;
    std::vector<float> grassvertex;
};
} // GameEngine
