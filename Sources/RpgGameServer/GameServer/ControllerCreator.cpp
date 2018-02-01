#include "ControllerCreator.h"
#include "../../Common/Controllers/CharacterController/Character.h"

namespace GameServer
{
	void CreateCharacterController(Hero::BaseHeroPtr hero, common::Controllers::Types type)
	{
		//CharacterController(common::Transform& transform, float runSpeed, float turnSpeed, float jumpPower);
		hero->AddController(std::make_shared<common::Controllers::CharacterController>(hero->context_.transform_, hero->context_.stats_.runSpeed, hero->context_.stats_.turnSpeed, hero->context_.stats_.jumpPower));
	}
} // GameServer

