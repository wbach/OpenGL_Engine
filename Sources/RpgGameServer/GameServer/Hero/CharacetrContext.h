#pragma once
#include "Stats.h"
#include "../../Common/Transform.h"

namespace GameServer
{
	namespace Hero
	{
		struct CharacterContext
		{
			Stats			  stats_;
			common::Transform transform_;
			uint32			  mapId;
		};
	} // Hero
} // GameServer
