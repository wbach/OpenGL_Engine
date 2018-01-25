#pragma once
#include "Types.h"
#include <SDL2/SDL_net.h>
#include "SDLNetWrapper.h"
#include "optional.hpp"

namespace UtilsNetwork
{
	struct UserData
	{
		TCPsocket socket;
		uint32 timeout;
		uint32 id;
		bool authenticated;
		wb::optional<Timepoint>	connectionFailsStart;

		UserData()
			: socket(nullptr)
			, timeout(0)
			, authenticated(false)
		{
			static int s_id = 0;
			id = s_id++;
		}

		bool isSocketReady()
		{
			return sdlNetWrapper.SocketReady((SDLNet_GenericSocket) socket);
		}

		Network::SDLNetWrapper sdlNetWrapper;

		virtual ~UserData() {}
	};
} // UtlisNetwork