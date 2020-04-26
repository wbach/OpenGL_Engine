#include "ModelWrapper.h"

#include <Logger/Log.h>
#include <Types.h>

#include "Model.h"

namespace GameEngine
{
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
        ERROR_LOG("Can not update, model not found");
    }
}
Model* ModelWrapper::Get(LevelOfDetail lvl)
{
    std::lock_guard<std::mutex> lk(mutex_);
    return models_.count(lvl) > 0 ? models_.at(lvl) : nullptr;
}
const std::unordered_map<LevelOfDetail, Model*>& ModelWrapper::Get() const
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
}  // namespace GameEngine
