#include "HeroFactory.hpp"
#include "Classes/Knight.h"

namespace GameServer
{
	namespace HeroFactory
	{
		Hero::BaseHeroPtr HeroFactory::CreateKnight()
		{
			return std::make_shared<Hero::Knight>();
		}
	} // Hero
} // GameServer