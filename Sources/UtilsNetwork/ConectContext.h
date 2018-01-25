#pragma once
#include "Types.h"
#include "User.h"

#include <string>
#include <vector>
#include <memory>
#include <SDL2/SDL_net.h>
#include <unordered_map>

namespace Network
{
	typedef std::unordered_map<uint32, std::shared_ptr<UtilsNetwork::UserData>> Users;

	struct ConectContext
	{
		Users users;
		uint32			 maxClients;
		uint32			 port;
		IPaddress		 serverIP;
		TCPsocket		 socket;
		SDLNet_SocketSet socketSet;
		std::string		 serverName;		

		ConectContext()
			: ConectContext(30, 1991)
		{
		}

		ConectContext(uint32 maxClients, uint32 port)
			: maxClients(maxClients)
			, port(port)
		{
		}
	};
}