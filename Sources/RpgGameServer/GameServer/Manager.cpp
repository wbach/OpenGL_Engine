#include "Manager.h"

namespace GameServer
{
	Hero::BaseHeroPtr Manager::GetHero(uint32 id)
	{
		for (auto& m : maps_)
		{
			auto character = m.GetCharacter(id);
			if (character != nullptr)
				return character;
		}
		return nullptr;
	}
} // GameServer
