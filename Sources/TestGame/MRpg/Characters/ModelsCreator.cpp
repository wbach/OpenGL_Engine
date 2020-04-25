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
ModelWrapper ModelsCreator::CreateHero(uint32 classId)
{
    ModelWrapper modelWrapper;

    if (resourceManager_ == nullptr)
    {
        ERROR_LOG("ModelsCreator::CreateHero resourceManager is nullptr.");
        return modelWrapper;
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

    auto model = resourceManager_->LoadModel("Meshes/DaeAnimationExample/CharacterRunning.dae");
    modelWrapper.Add(model, LevelOfDetail::L1);
    return modelWrapper;
}
}  // MmmoRpg
