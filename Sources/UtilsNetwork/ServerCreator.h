#pragma once
#include "NetworkCreator.h"

namespace Network
{
	class ServerCreator : public CNetworkCreator
	{
	public:
		ServerCreator(std::shared_ptr<ISDLNetWrapper> sdlNetWrapper = std::shared_ptr<ISDLNetWrapper>(new SDLNetWrapper()));
		ServerCreator(ISDLNetWrapper* sdlNetWrapper);
		ConectContext Create(uint32 maxClients, uint32 port);

	};
}