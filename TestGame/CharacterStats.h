#pragma once
#include "../GameEngine/Utils/Utils.h"

struct SCharacterStats
{
	int m_CurrentHealth = 60;
	int m_MaxHealth = 100;

	int m_CurrentMana = 10;
	int m_MaxMana = 10;

	int m_CurrentStamina = 50;
	int m_MaxStamina = 50;

	float m_Range = 1.f;
	float m_RunSpeed = Utils::KmToMs(45.f);
	float m_WalkSpeed = Utils::KmToMs(5.f);
	float m_CurrentMoveSpeed = 0.f;
	float m_CurrentTurnSpeed = 0.f;
	float m_JumpPower = 25.f;
	float m_TurnSpeed = 160.f;

	int m_Strength = 10;
	int m_Agility = 10;
	int m_Vitality = 10;
	int m_Energy = 10;
};