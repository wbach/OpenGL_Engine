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
    void Add(Model*, LevelOfDetail);
    void Update(Model*, LevelOfDetail);
    Model* Get(LevelOfDetail lvl = LevelOfDetail::L1);
    const std::unordered_map<LevelOfDetail, Model*>& Get() const;

private:
    std::unordered_map<LevelOfDetail, Model*> models_;
};
}  // GameEngine
