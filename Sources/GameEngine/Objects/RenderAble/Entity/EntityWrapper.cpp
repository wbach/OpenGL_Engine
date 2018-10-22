#include "EntityWrapper.h"

namespace GameEngine
{
EntityWrapper::EntityWrapper(IResourceManager* manager)
    : manager_(manager)
    , entity_(nullptr)
{
}

EntityWrapper::EntityWrapper(ModelWrapper modelWrapper)
    : manager_(nullptr)
    , entity_(nullptr)
    , modelWrapper_(modelWrapper)
{
}

Entity* EntityWrapper::Get()
{
    if (entity_ != nullptr)
        return entity_.get();

    if (manager_ == nullptr)
    {
        entity_ = std::make_unique<Entity>(modelWrapper_);
    }
    else
    {
        entity_ = std::make_unique<Entity>(manager_);
    }

    return entity_.get();
}
}  // namespace GameEngine
