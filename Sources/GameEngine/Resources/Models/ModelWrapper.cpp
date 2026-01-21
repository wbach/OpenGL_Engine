#include "ModelWrapper.h"

#include <Logger/Log.h>
#include <Types.h>

#include "GameEngine/Engine/Configuration.h"
#include "Model.h"

namespace GameEngine
{
ModelWrapper::ModelWrapper(ModelWrapper&& other)
    : models_(std::move(other.models_))
{
}
void ModelWrapper::Add(Model* model, LevelOfDetail lvl)
{
    std::lock_guard<std::mutex> lk(mutex_);
    models_.insert({lvl, model});
}
void ModelWrapper::Update(Model* model, LevelOfDetail lvl)
{
    std::lock_guard<std::mutex> lk(mutex_);
    if (models_.count(lvl))
    {
        models_.at(lvl) = model;
    }
    else
    {
        LOG_ERROR << "Can not update, model not found";
    }
}
Model* ModelWrapper::Get(LevelOfDetail lvl)
{
    std::lock_guard<std::mutex> lk(mutex_);
    auto iter = models_.find(lvl);
    return iter != models_.end() ? iter->second : nullptr;
}
const Model* ModelWrapper::Get(LevelOfDetail lvl) const
{
    std::lock_guard<std::mutex> lk(mutex_);
    auto iter = models_.find(lvl);
    return iter != models_.end() ? iter->second : nullptr;
}
Model* ModelWrapper::get(DistanceToCamera distance)
{
    std::lock_guard<std::mutex> lk(mutex_);

    if (distance < EngineConf.renderer.lodDistance0)
    {
        auto iter = models_.find(LevelOfDetail::L1);
        if (iter != models_.end())
            return iter->second;
    }
    else if (distance < EngineConf.renderer.lodDistance1)
    {
        auto iter = models_.find(LevelOfDetail::L2);
        if (iter != models_.end())
            return iter->second;
    }
    else
    {
        auto iter = models_.find(LevelOfDetail::L3);
        if (iter != models_.end())
            return iter->second;
    }

    for (auto& pair : models_)
        return pair.second;

    return nullptr;
}
const std::unordered_map<LevelOfDetail, Model*>& ModelWrapper::GetAll() const
{
    return models_;
}
std::vector<Model*> ModelWrapper::PopModels()
{
    std::lock_guard<std::mutex> lk(mutex_);

    std::vector<Model*> result;
    for (auto& model : models_)
        result.push_back(model.second);
    models_.clear();

    return result;
}
void ModelWrapper::clear()
{
    models_.clear();
}

void ModelWrapper::clear(LevelOfDetail lod)
{
    models_.erase(lod);
}
ModelWrapper& ModelWrapper::operator=(const ModelWrapper& other)
{
    if (this != &other)
    {
        models_ = other.models_;
    }
    return *this;
}
}  // namespace GameEngine
