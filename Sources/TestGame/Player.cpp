#include "Player.h"

CPlayer::CPlayer(CInputManager *input_manager, CResourceManager &manager, const glm::vec3 &normalized_scale, const std::string &filename)
    : CEntity(manager, normalized_scale, filename)
    , inputManager(input_manager)
{}

void CPlayer::SetAction(CharacterActions::Type action)
{
    action = action;
}

void CPlayer::SetPosition(const glm::vec3 & position)
{
    worldTransform.SetPosition(position);
}

void CPlayer::Move(const float & delta_time)
{
	CheckInputs();
    worldTransform.IncreaseRotation(0, characterStats.currentTurnSpeed * delta_time, 0);
	float distance = characterStats.currentMoveSpeed * delta_time;
    float dx = static_cast<float>(distance * sin(Utils::ToRadians(worldTransform.GetRotation().y)));
    float dz = static_cast<float>(distance * cos(Utils::ToRadians(worldTransform.GetRotation().y)));
    worldTransform.IncrasePosition(dx, 0.f, dz);

	upwardsSpeed += -10.f* delta_time;
    worldTransform.IncrasePosition(0.f, upwardsSpeed * 0.01f, 0.f);
}

void CPlayer::Jump()
{
	if (isGrounded)
	{
		upwardsSpeed = characterStats.jumpPower;
		isGrounded = false;
	}
}

void CPlayer::CheckInputs()
{
	if (inputManager == nullptr) return;

	bool move_key_pres = false;

	if (inputManager->GetKey(GameActions::MOVE_FORWARD))
	{
		characterStats.currentMoveSpeed = characterStats.runSpeed;
		SetAction(CharacterActions::RUN);
		move_key_pres = true;
	}
	else if (inputManager->GetKey(GameActions::MOVE_BACKWARD))
	{
		characterStats.currentMoveSpeed = -characterStats.runSpeed;
		SetAction(CharacterActions::RUN);
		move_key_pres = true;
	}
	else if (inputManager->GetKey(GameActions::ATTACK_1))
	{
		SetAction(CharacterActions::ATTACK_1);
	}
	else if (inputManager->GetKey(GameActions::ATTACK_2))
	{
		SetAction(CharacterActions::ATTACK_2);
	}
	else if (inputManager->GetKey(GameActions::ATTACK_3))
	{
		SetAction(CharacterActions::ATTACK_3);
	}
	else
	{
		SetAction(CharacterActions::IDLE);
	}

	if (!move_key_pres)
		characterStats.currentMoveSpeed = 0;

	if (inputManager->GetKey(KeyCodes::D))
	{
        characterStats.currentTurnSpeed = -characterStats.turnSpeed;
	}
	else if (inputManager->GetKey(KeyCodes::A))
	{
        characterStats.currentTurnSpeed = characterStats.turnSpeed;
	}
	else
	{
		characterStats.currentTurnSpeed = 0;
	}

	if (inputManager->GetKey(GameActions::JUMP))
	{
		Jump();
	}
}
