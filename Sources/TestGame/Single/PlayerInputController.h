#pragma once
#include "Common/Hero/Stats.h"
#include "Common/Controllers/CharacterController/Character.h"
#include "Components/Animation/Animator.h"
#include <memory>

class CGameObject;

namespace GameEngine
{
	class InputManager;
}

class PlayerInputController
{
public:
	PlayerInputController(GameEngine::Components::Animator* animator, GameEngine::InputManager* manager, common::Controllers::CharacterController* characterController);

private:
	void SubscribeForPushActions();
	void SubscribeForPopActions();
	bool FindState(common::Controllers::CharacterActions::Type type);
	void AddState(common::Controllers::CharacterActions::Type state);
	void RemoveState(common::Controllers::CharacterActions::Type state);
	void SetRunAnim();
	void SetIdleAnim();

private:
	GameEngine::Components::Animator* animator_;
	GameEngine::InputManager* inputManager_;
	common::Controllers::CharacterController* characterController_;
	std::list<common::Controllers::CharacterActions::Type> states_;
};

typedef std::unique_ptr<PlayerInputController> PlayerControllerPtr;
