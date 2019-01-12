#include "ModelWrapper.h"
#include "Model.h"
#include "Types.h"

namespace GameEngine
{
void ModelWrapper::Add(ModelRawPtr model, LevelOfDetail lvl)
{
    models_.insert({ lvl, model });
}
ModelRawPtr ModelWrapper::Get(LevelOfDetail lvl)
{
    if (models_.count(lvl) > 0)
        return models_.at(lvl);

    if (!models_.empty())
    {
        for (const auto& model : models_)
            return model.second;
    }

    return nullptr;
}
}  // GameEngine
