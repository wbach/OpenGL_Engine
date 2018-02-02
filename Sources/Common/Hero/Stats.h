#pragma once
#include "Types.h"
#include "Utils.h"
#include "../Transform.h"

namespace common
{
	namespace Hero
	{
		struct CommonStats
		{
			float runSpeed = Utils::KmToMs(45.f);
			float walkSpeed = Utils::KmToMs(5.f);
			float jumpPower = 25.f;
			float turnSpeed = 160.f;
			float attackSpeed = 1.f;
			//float currentMoveSpeed = 0.f;
			//float currentTurnSpeed = 0.f;
		};

		struct CharacterData
		{
			uint8  lvl = 1;
			uint8  classId;
			uint32 mapId;
			std::string name;
		};

		struct Stats
		{
			uint32 currentExp = 0;
			uint32 expToNextLvl = 100;

			uint16 currentHealth = 60;
			uint16 maxHealth = 100;

			uint16 currentMana = 10;
			uint16 maxMana = 10;

			uint16 currentStamina = 50;
			uint16 maxStamina = 50;

			float range = 1.f;

			uint16 strength = 10;
			uint16 agility = 10;
			uint16 vitality = 10;
			uint16 energy = 10;			
		};
	} // Hero
} // GameServer
