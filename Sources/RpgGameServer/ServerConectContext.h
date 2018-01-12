#pragma once
#include "Types.h"
#include <string>
#include <SDL2/SDL_net.h>

namespace Network
{
	struct SServerConectContext
	{	
		uint32			 maxClients;
		uint32			 port;
		IPaddress		 serverIP;
		TCPsocket		 serverSocket;
		SDLNet_SocketSet socketSet;

		SServerConectContext()
			: SServerConectContext(30, 1991)
		{
		}

		SServerConectContext(uint32 maxClients, uint32 port)
			: maxClients(maxClients)
			, port(port)
		{
		}
	};
}