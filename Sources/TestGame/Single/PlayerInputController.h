#pragma once
#include "Common/Hero/Stats.h"
#include "Common/Controllers/CharacterController/Character.h"
#include <memory>

namespace GameEngine
{
	class InputManager;
}

class PlayerInputController
{
public:
	PlayerInputController(GameEngine::InputManager* manager, common::Controllers::CharacterController* characterController);

private:
	void SubscribeForPushActions();
	void SubscribeForPopActions();
	bool FindState(common::Controllers::CharacterActions::Type type);

private:
	GameEngine::InputManager* inputManager_;
	common::Controllers::CharacterController* characterController_;
	std::list<common::Controllers::CharacterActions::Type> states_;
};

typedef std::unique_ptr<PlayerInputController> PlayerControllerPtr;
