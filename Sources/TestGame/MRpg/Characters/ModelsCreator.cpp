#include "ModelsCreator.h"
#include "GameEngine/Resources/Models/ModelFactory.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "Common/Hero/HeroClassesTypes.h"
#include "Logger/Log.h"

using namespace common::Hero;

namespace MmmoRpg
{
	ModelsCreator::ModelsCreator(CResourceManager* resourceManager)
		: resourceManager_(resourceManager)
	{
	}
	GameEngine::ModelWrapper ModelsCreator::CreateHero(uint32 classId)
	{
		GameEngine::ModelWrapper modelWrapper;

		if (resourceManager_ == nullptr)
		{
			Log("ModelsCreator::CreateHero resourceManager is nullptr.");
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

		auto model = GameEngine::LoadModel(resourceManager_, vec3(0, 2, 0), "Meshes/DaeAnimationExample/CharacterRunning.dae");
		modelWrapper.Add(model, GameEngine::LevelOfDetail::L1);
		return modelWrapper;
	}
} // MmmoRpg