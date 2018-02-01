#pragma once
#include "../../../Common/Transform.h"

namespace GameServer
{
	namespace Hero
	{
		struct CharacterData
		{
			uint8  lvl = 1;
			uint8  classId;
			uint32 mapId;
			std::string name;
		};
	} // Hero
} // GameServer
