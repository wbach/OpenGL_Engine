#pragma once
#include  "../Common/Controllers/ControllersTypes.h"
#include "Hero/BaseHero.h"

namespace GameServer
{
	void CreateCharacterController(Hero::BaseHeroPtr, Controllers::Types type);
} // GameServer