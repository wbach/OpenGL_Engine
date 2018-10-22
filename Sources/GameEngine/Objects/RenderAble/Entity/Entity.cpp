#include "Entity.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/Models/ModelFactory.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "Logger/Log.h"

namespace GameEngine
{
Entity::Entity(IResourceManager* manager)
    : manager_(manager)
{
}

Entity::Entity(const ModelWrapper& modelWrapper)
    : manager_(nullptr)
    , modelWrapper_(modelWrapper)
{
}

ModelRawPtr Entity::GetModel(LevelOfDetail i)
{
    return modelWrapper_.Get(i);
}

void Entity::AddModel(const std::string& filename, LevelOfDetail i)
{
    if (filename.empty() || manager_ == nullptr)
        return;

    auto model = GameEngine::LoadModel(manager_, filename);
    modelWrapper_.Add(model, i);
}

void Entity::SetResourceManager(IResourceManager *manager)
{
    manager_ = manager;
}

}  // GameEngine
