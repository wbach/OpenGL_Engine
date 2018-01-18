#include "Knight.h"
namespace GameServer
{
	namespace Hero
	{
		Hero::Knight::Knight()
		{
		}
		Knight::Knight(const std::string & name)
			: BaseHero("knight_example_model.obj", name)
		{
		}
	} // Hero
} // GameServer