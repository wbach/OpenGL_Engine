#pragma once
#include <memory>
#include "GameEngine/Resources/Models/ModelWrapper.h"

namespace GameEngine
{
class IResourceManager;
}  //  namespace GameEngine

namespace MmmoRpg
{
class ModelsCreator
{
public:
    ModelsCreator(GameEngine::IResourceManager* resourceManager);
    GameEngine::ModelWrapper CreateHero(uint32 classId);

private:
    GameEngine::IResourceManager* resourceManager_;
};
typedef std::unique_ptr<ModelsCreator> ModelsCreatorPtr;
} // namespace MmmoRpg
