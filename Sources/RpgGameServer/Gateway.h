#pragma once
#include "Types.h"
#include "ServerConectContext.h"

namespace Network
{
	class CGateway
	{
	public:
		void StartServer(uint32 maxClients, uint32 port);

	private:
		SServerConectContext context_;
	};
}