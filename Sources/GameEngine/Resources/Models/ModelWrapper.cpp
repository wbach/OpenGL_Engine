#include "ModelWrapper.h"
#include "Model.h"
#include "Types.h"

namespace GameEngine
{
void ModelWrapper::Add(ModelRawPtr model, LevelOfDetail lvl)
{
    models_.insert({lvl, model});
}
void ModelWrapper::Update(ModelRawPtr model, LevelOfDetail lvl)
{
    if (models_.count(lvl))
    {
        models_.at(lvl) = model;
    }
}
ModelRawPtr ModelWrapper::Get(LevelOfDetail lvl)
{
    return models_.count(lvl) > 0 ? models_.at(lvl) : nullptr;
}
const std::unordered_map<LevelOfDetail, ModelRawPtr>& ModelWrapper::Get() const
{
    return models_;
}
}  // namespace GameEngine
