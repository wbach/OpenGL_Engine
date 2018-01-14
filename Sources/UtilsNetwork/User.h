#pragma once
#include "Types.h"
#include <SDL2/SDL_net.h>

namespace UtilsNetwork
{
	struct UserData
	{
		TCPsocket socket;
		uint32 timeout;
		uint32 id;
		bool authenticated;

		UserData()
			: socket(nullptr)
			, timeout(0)
			, authenticated(false)
		{
			static int s_id = 0;
			id = s_id++;
		}
		virtual ~UserData() {}
	};
} // UtlisNetwork