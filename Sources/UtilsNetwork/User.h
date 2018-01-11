#pragma once
#include "Types.h"
#include <SDL2/SDL_net.h>

namespace UtlisNetwork
{
	struct UserData
	{
		TCPsocket socket;
		uint32 timeout;
		uint32 id;

		UserData()
			: socket(nullptr)
			, timeout(0)
		{
			static int s_id = 0;
			id = s_id++;
		}
	};
} // UtlisNetwork