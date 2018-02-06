#pragma once
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include <memory>

class CResourceManager;

namespace MmmoRpg
{
	class ModelsCreator
	{
	public:
		ModelsCreator(CResourceManager* resourceManager);
		GameEngine::ModelWrapper CreateHero(uint32 classId);

	private:
		CResourceManager* resourceManager_;
	};
	typedef std::unique_ptr<ModelsCreator> ModelsCreatorPtr;
}