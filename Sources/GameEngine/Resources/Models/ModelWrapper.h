#pragma once
#include <Mutex.hpp>
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
    ModelWrapper() = default;
    ModelWrapper(ModelWrapper&&);
    ModelWrapper& operator=(const ModelWrapper&);

    using DistanceToCamera = float;
    void Add(Model*, LevelOfDetail);
    void Update(Model*, LevelOfDetail);
    Model* Get(LevelOfDetail = LevelOfDetail::L1);
    const Model* Get(LevelOfDetail = LevelOfDetail::L1) const;
    Model* get(DistanceToCamera);
    const std::unordered_map<LevelOfDetail, Model*>& GetAll() const;
    std::vector<Model*> PopModels();
    void clear();
    void clear(LevelOfDetail);

private:
    mutable std::mutex mutex_;
    std::unordered_map<LevelOfDetail, Model*> models_;
};
}  // namespace GameEngine
