#pragma once
#include "../../../Common/Hero/Stats.h"
#include "../../../Common/Hero/HeroClassesTypes.h"
#include "../../../Common/Controllers/ControllersTypes.h"
#include "../../../GameEngine/Objects/RenderAble/Entity/EntityWrapper.h"
#include <unordered_map>

namespace common
{
	namespace Controllers
	{
		class IController;
	} // Controllers
} // common

namespace MmmoRpg
{
	typedef std::unordered_map<common::Hero::HeroClassesTypes, GameEngine::ModelWrapper> CharacterModelsMap;
	typedef std::unordered_map<common::Controllers::Types, std::shared_ptr<common::Controllers::IController>> ControllersMap;

	class NetworkCharacter 
	{
	public:
		NetworkCharacter(uint32 id, const vec3& scale, const common::Hero::CommonStats&, GameEngine::ModelWrapper modelWrapper);
		std::shared_ptr<common::Controllers::IController> GetControllerByType(common::Controllers::Types type);
		uint32 GetId();
		CEntity* GetEntity();

	private:
		uint32 id_;
		CEntityWrapper enitityWrapper_;
		ControllersMap controllers_;
		common::Hero::CommonStats stats_;
	};
} // MmmoRpg