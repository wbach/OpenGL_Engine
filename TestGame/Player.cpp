#include "Player.h"

void CPlayer::SetAction(CharacterActions::Type action)
{
	m_Action = action;
}

void CPlayer::SetPosition(const glm::vec3 & position)
{
	m_WorldTransform.SetPosition(position);
}

void CPlayer::Move(const float & delta_time)
{
	CheckInputs();
	m_WorldTransform.IncreaseRotation(0, m_CharacterStats.m_CurrentTurnSpeed * delta_time, 0);
	float distance = m_CharacterStats.m_CurrentMoveSpeed * delta_time;
	float dx = static_cast<float>(distance * sin(Utils::ToRadians(m_WorldTransform.GetRotation().y)));
	float dz = static_cast<float>(distance * cos(Utils::ToRadians(m_WorldTransform.GetRotation().y)));
	m_WorldTransform.IncrasePosition(dx, 0.f, dz);

	upwardsSpeed += -10.f* delta_time;
	m_WorldTransform.IncrasePosition(0.f, upwardsSpeed * 0.01f, 0.f);
}

void CPlayer::Jump()
{
	if (m_IsGrounded)
	{
		upwardsSpeed = m_CharacterStats.m_JumpPower;
		m_IsGrounded = false;
	}
}

void CPlayer::CheckInputs()
{
	if (m_InputManager == nullptr) return;

	bool move_key_pres = false;

	if (m_InputManager->GetKey(GameActions::MOVE_FORWARD))
	{
		m_CharacterStats.m_CurrentMoveSpeed = m_CharacterStats.m_RunSpeed;
		SetAction(CharacterActions::RUN);
		move_key_pres = true;
	}
	else if (m_InputManager->GetKey(GameActions::MOVE_BACKWARD))
	{
		m_CharacterStats.m_CurrentMoveSpeed = -m_CharacterStats.m_RunSpeed;
		SetAction(CharacterActions::RUN);
		move_key_pres = true;
	}
	else if (m_InputManager->GetKey(GameActions::ATTACK_1))
	{
		SetAction(CharacterActions::ATTACK_1);
	}
	else if (m_InputManager->GetKey(GameActions::ATTACK_2))
	{
		SetAction(CharacterActions::ATTACK_2);
	}
	else if (m_InputManager->GetKey(GameActions::ATTACK_3))
	{
		SetAction(CharacterActions::ATTACK_3);
	}
	else
	{
		SetAction(CharacterActions::IDLE);
	}

	if (!move_key_pres)
		m_CharacterStats.m_CurrentMoveSpeed = 0;

	if (m_InputManager->GetKey(KeyCodes::D))
	{
		m_CharacterStats.m_CurrentTurnSpeed = -m_CharacterStats.m_TurnSpeed;
	}
	else if (m_InputManager->GetKey(KeyCodes::A))
	{
		m_CharacterStats.m_CurrentTurnSpeed = m_CharacterStats.m_TurnSpeed;
	}
	else
	{
		m_CharacterStats.m_CurrentTurnSpeed = 0;
	}

	if (m_InputManager->GetKey(GameActions::JUMP))
	{
		Jump();
	}
}
