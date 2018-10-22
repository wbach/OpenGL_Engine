#pragma once
#include <string>
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"

namespace GameEngine
{
class IResourceManager;

class Entity : public GameObject
{
public:
    Entity(IResourceManager* manager);
    Entity(const ModelWrapper& modelWrapper);
    void AddModel(const std::string& filename, LevelOfDetail i);
    void SetResourceManager(IResourceManager* manager);
    ModelRawPtr GetModel(GameEngine::LevelOfDetail i = LevelOfDetail::L1);

private:
    IResourceManager* manager_;
    GameEngine::ModelWrapper modelWrapper_;
};

}  // GameEngine
