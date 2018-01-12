#pragma once
#include "ServerConectContext.h"

namespace Network
{
	class CNetworkCreator
	{
	public:
		SServerConectContext Create(uint32 maxClients, uint32 port);

	private:
		void Init();
		void AllocSocketSet();
		void ResolveHost();
		void OpenTcp();

	private:
		SServerConectContext context_;
	};
}