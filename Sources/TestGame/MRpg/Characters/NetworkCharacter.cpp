#include "NetworkCharacter.h"
#include "../../../Common/Controllers/ControllersFactory.h"
#include "../../../Common/Controllers/IController.h"

namespace MmmoRpg
{
	NetworkCharacter::NetworkCharacter(uint32 id, const vec3 & scale, const common::Hero::CommonStats& stats, GameEngine::ModelWrapper modelWrapper)
		: enitityWrapper_(scale, modelWrapper)
		, id_(id)
		, stats_(stats)
	{
		auto characterController = common::Controllers::CreateCharacterController(enitityWrapper_.Get()->worldTransform, stats_);
		controllers_[common::Controllers::Types::CharacterControllerType] = characterController;
	}

	std::shared_ptr<common::Controllers::IController> NetworkCharacter::GetControllerByType(common::Controllers::Types type)
	{
		if (controllers_.count(type) == 0)
			return nullptr;

		return controllers_[type];
	}
	void NetworkCharacter::UpdateControllers(float dt)
	{
		for (auto& controller : controllers_)
			controller.second->Update(dt);
	}
	uint32 NetworkCharacter::GetId()
	{
		return id_;
	}
	CEntity* NetworkCharacter::GetEntity()
	{
		return enitityWrapper_.Get();
	}
} // MmmoRpg