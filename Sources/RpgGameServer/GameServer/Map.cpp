#include "Map.h"

namespace GameServer
{
	Hero::BaseHeroPtr Map::GetCharacter(uint32 id)
	{
		return characters_[id];
	}
} // GameServer