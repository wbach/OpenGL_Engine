#include "ControllerCreator.h"
#include "../Common/Controllers/Character.h"

namespace GameServer
{
	void CreateCharacterController(Hero::BaseHeroPtr hero, Controllers::Types type)
	{
		//CharacterController(common::Transform& transform, float runSpeed, float turnSpeed, float jumpPower);
		hero->AddController(std::make_shared<Controllers::CharacterController>(hero->context_.transform_, hero->context_.stats_.runSpeed, hero->context_.stats_.turnSpeed, hero->context_.stats_.jumpPower));
	}
} // GameServer

