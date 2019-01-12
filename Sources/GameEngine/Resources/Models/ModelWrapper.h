#pragma once
#include <unordered_map>
#include "Model.h"

namespace GameEngine
{
enum LevelOfDetail
{
    L1 = 0,
    L2,
    L3
};

class ModelWrapper
{
public:
    ModelWrapper()
    {
    }
    void Add(ModelRawPtr model, LevelOfDetail lvl);
    ModelRawPtr Get(LevelOfDetail lvl = LevelOfDetail::L1);

private:
    std::unordered_map<LevelOfDetail, ModelRawPtr> models_;
};
}  // GameEngine
