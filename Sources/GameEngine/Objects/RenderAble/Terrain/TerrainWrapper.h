#pragma once
#include <memory>
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Objects/ObjectBuilder.h"

namespace GameEngine
{
class Terrain;

class TerrainWrapper : public GameObject
{
public:
    TerrainWrapper(IResourceManager* manager, TerrainTexturesMap texture);
    ~TerrainWrapper();
    virtual wb::optional<vec3> CollisionDetection(const vec3&) override;
    Terrain* Get();

private:
    std::unique_ptr<Terrain> terrain;
    TerrainTexturesMap texturesMap;
    IResourceManager* manager;
};
}  // GameEngine
