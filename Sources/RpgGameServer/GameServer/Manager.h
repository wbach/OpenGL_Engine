#pragma once
#include "Map.h"

namespace GameServer
{
	typedef std::vector<Map> Maps;

	class Manager
	{
	public:
		Hero::BaseHeroPtr GetHero(uint32 id);

	private:
		Maps maps_;
	};
} // GameServer