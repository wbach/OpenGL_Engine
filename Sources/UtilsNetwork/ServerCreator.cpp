#include "ServerCreator.h"
#include "Logger/Log.h"
#include "NetworkUtils.h"

namespace Network
{
	ServerCreator::ServerCreator(std::shared_ptr<ISDLNetWrapper> sdlNetWrapper)
		: CNetworkCreator(sdlNetWrapper)
	{
	}

	ServerCreator::ServerCreator(ISDLNetWrapper* sdlNetWrapper)
		: CNetworkCreator(sdlNetWrapper)
	{
	}

	ConectContext ServerCreator::Create(uint32 maxClients, uint32 port)
	{
		context_.maxClients = maxClients;
		context_.port		= port;

		if (!Init()) return context_;
		if (!AllocSocketSet(context_.maxClients + 1)) return context_;
		if (!ResolveHost()) return context_;
		if (!OpenTcp()) return context_;
		if (!AddSocketTcp()) return context_;
		DEBUG_LOG("Connection is open.");
		isCreated = true;

		return context_;
	}

} // Network
