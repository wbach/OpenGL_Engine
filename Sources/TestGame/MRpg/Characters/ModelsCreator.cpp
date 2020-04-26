#include "ModelsCreator.h"
#include "Common/Hero/HeroClassesTypes.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "Logger/Log.h"

using namespace GameEngine;
using namespace common::Hero;

namespace MmmoRpg
{
ModelsCreator::ModelsCreator(IResourceManager *resourceManager)
    : resourceManager_(resourceManager)
{
}
void ModelsCreator::CreateHero(uint32 classId, GameEngine::ModelWrapper& model)
{
    ModelWrapper modelWrapper;

    if (resourceManager_ == nullptr)
    {
        ERROR_LOG("ModelsCreator::CreateHero resourceManager is nullptr.");
        return;
    }

    switch (classId)
    {
        case HeroClassesTypes::KNIGHT:
            break;
        case HeroClassesTypes::ELF:
            break;
        case HeroClassesTypes::WIZARD:
            break;
        default:
        {
        }
        break;
    }

    auto l1Model = resourceManager_->LoadModel("Meshes/DaeAnimationExample/CharacterRunning.dae");
    model.Add(l1Model, LevelOfDetail::L1);
}
}  // MmmoRpg
