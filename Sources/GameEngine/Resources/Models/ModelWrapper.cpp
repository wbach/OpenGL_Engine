#include "ModelWrapper.h"
#include "Model.h"
#include "Types.h"

namespace GameEngine
{
void ModelWrapper::Add(Model* model, LevelOfDetail lvl)
{
    models_.insert({lvl, model});
}
void ModelWrapper::Update(Model* model, LevelOfDetail lvl)
{
    if (models_.count(lvl))
    {
        models_.at(lvl) = model;
    }
}
Model* ModelWrapper::Get(LevelOfDetail lvl)
{
    return models_.count(lvl) > 0 ? models_.at(lvl) : nullptr;
}
const std::unordered_map<LevelOfDetail, Model*>& ModelWrapper::Get() const
{
    return models_;
}
}  // namespace GameEngine
