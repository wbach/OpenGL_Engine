#pragma once
#include "../../Hero/Stats.h"
#include "../../../Common/Transform.h"

namespace GameServer
{
	struct CharacterInDataBase
	{
		uint32 mapId = 0;
		Hero::Stats stats;
		//common::Transform transform;
	};
}