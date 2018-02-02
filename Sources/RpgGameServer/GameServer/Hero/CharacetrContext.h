#pragma once
#include "../../../Common/Hero/Stats.h"
#include "../../../Common/Transform.h"

using namespace common;
using namespace common::Hero;

namespace GameServer
{
	namespace Hero
	{
		struct CharacterContext
		{
			Stats stats_;
			CharacterData data_;
			Transform transform_;
			CommonStats commonStats_;

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
