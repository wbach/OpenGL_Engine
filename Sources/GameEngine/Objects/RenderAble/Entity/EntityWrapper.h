#pragma once
#include <memory>
#include "Entity.h"

namespace GameEngine
{
class EntityWrapper
{
public:
    EntityWrapper(IResourceManager* manager);
    EntityWrapper(ModelWrapper modelWrapper);
    Entity* Get();

private:
    vec3 normalizeScale_;
    IResourceManager* manager_;
    std::unique_ptr<Entity> entity_;

    ModelWrapper modelWrapper_;
};
}  // namespace GameEngine
