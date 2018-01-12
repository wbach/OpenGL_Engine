#include "Gateway.h"
#include "ServerCreator.h"
#include "Logger/Log.h"
#include <SDL2/SDL_net.h>

namespace Network
{
	void CGateway::StartServer(uint32 maxClients, uint32 port)
	{
		context_ = CNetworkCreator().Create(maxClients, port);
	}
}


