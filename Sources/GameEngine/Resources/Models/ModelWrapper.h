#pragma once
#include <unordered_map>
#include "Model.h"
#include <Mutex.hpp>

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
    using DistanceToCamera = float;
    void Add(Model*, LevelOfDetail);
    void Update(Model*, LevelOfDetail);
    Model* Get(LevelOfDetail lvl = LevelOfDetail::L1);
    Model* get(DistanceToCamera);
    const std::unordered_map<LevelOfDetail, Model*>& GetAll() const;
    std::vector<Model*> PopModels();
    void clear();

private:
    std::mutex mutex_;
    std::unordered_map<LevelOfDetail, Model*> models_;
};
}  // GameEngine
