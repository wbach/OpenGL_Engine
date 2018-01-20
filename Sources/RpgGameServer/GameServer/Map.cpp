#include "Map.h"

namespace GameServer
{
	Hero::BaseHeroPtr Map::GetCharacter(uint32 id)
	{
		if (characters_.count(id) == 0)
			return nullptr;

		return characters_[id];
	}
	void Map::AddCharacter(uint32 characterId, Hero::BaseHeroPtr hero)
	{
		characters_[characterId] = hero;
	}
	void Map::UpdateAllControllers(float dt)
	{
		for (auto& c : characters_)
			c.second->UpdateAllControllers(dt);		
	}
} // GameServer