#pragma once
#include "ConectContext.h"
#include "ISDLNetWrapper.h"
#include "SDLNetWrapper.h"

namespace Network
{
	class CNetworkCreator
	{
	public:
		CNetworkCreator(std::shared_ptr<ISDLNetWrapper> sdlNetWrapper = std::shared_ptr<ISDLNetWrapper>(new SDLNetWrapper()));
		CNetworkCreator(ISDLNetWrapper* sdlNetWrapper);
		virtual ~CNetworkCreator();

	public:
		bool Init();
		bool AllocSocketSet(int count);
		bool ResolveHost(const char* hostName = nullptr);
		bool ResolveIp();
		bool OpenTcp();
		bool AddSocketTcp();

	protected:
		ConectContext context_;
		std::shared_ptr<ISDLNetWrapper> sdlNetWrapper_;
		bool isCreated;
	};
}