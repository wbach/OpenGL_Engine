#pragma once
#include "CharacterData.h"
#include "Stats.h"
#include "../../../Common/Transform.h"

namespace GameServer
{
	namespace Hero
	{
		struct CharacterContext
		{
			Stats stats_;
			CharacterData data_;
			common::Transform transform_;

			void AddExp(uint32 exp)
			{
				stats_.currentExp += exp;

				if (stats_.currentExp > stats_.expToNextLvl)
				{
					++data_.lvl;
					stats_.currentExp -= stats_.expToNextLvl;
					stats_.expToNextLvl += 500;
				}
			}
		};
	} // Hero
} // GameServer
