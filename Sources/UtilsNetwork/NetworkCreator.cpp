#include "ServerCreator.h"
#include "Logger/Log.h"
#include "NetworkUtils.h"

namespace Network
{
	CNetworkCreator::CNetworkCreator(std::shared_ptr<ISDLNetWrapper> sdlNetWrapper)
		: sdlNetWrapper_(sdlNetWrapper)
		, isCreated(false)
	{
	}

	CNetworkCreator::CNetworkCreator(ISDLNetWrapper* sdlNetWrapper)
		: sdlNetWrapper_(sdlNetWrapper)
		, isCreated(false)
	{
	}

	CNetworkCreator::~CNetworkCreator()
	{
		if (!isCreated)
			return;

		SDLNet_FreeSocketSet(context_.socketSet);
		SDLNet_Quit();
	}	

	bool CNetworkCreator::Init()
	{
		if (sdlNetWrapper_->Init() != -1)
			return true;

		Error("Failed to intialise SDL_net: " + std::string(sdlNetWrapper_->GetError()));
		return false;
	}

	bool CNetworkCreator::AllocSocketSet(int count)
	{
		context_.socketSet = sdlNetWrapper_->AllocSocketSet(count);

		if (context_.socketSet == nullptr)
		{
			Error("Failed to allocate the socket set:: " + std::string(sdlNetWrapper_->GetError()));
			return false;
		}
		
		std::string str = "Allocated socket set with size:  " + std::to_string(context_.maxClients + 1) + ", of which " + std::to_string(context_.maxClients) + " are availble for use by clients.";
		Log(str);
		return true;
	}

	bool CNetworkCreator::ResolveHost(const char* hostName)
	{
		int hostResolved = sdlNetWrapper_->ResolveHost(&context_.serverIP, hostName, context_.port);

		if (hostResolved == -1)
		{
			Error("Failed to resolve the server host: " + std::string(sdlNetWrapper_->GetError()));
			return false;
		}

		Log("Successfully resolved server host to IP: " + UtilsNetwork::IpAddressToString(context_.serverIP));
		return true;
	}

	bool CNetworkCreator::ResolveIp()
	{
		const char* ipResolved = sdlNetWrapper_->ResolveIP(&context_.serverIP);

		if (ipResolved == nullptr)
		{
			Error("Failed to resolve ip: " + std::string(sdlNetWrapper_->GetError()));
			return false;
		}

		Log("Ip resolved : " + std::string(ipResolved));
		return true;
	}

	bool CNetworkCreator::OpenTcp()
	{
		// Try to open the server socket
		context_.socket = sdlNetWrapper_->TCPOpen(&context_.serverIP);

		if (!context_.socket)
		{
			Error("Failed to open the server socket: " + std::string(sdlNetWrapper_->GetError()));
			return false;
		}

		Log("Sucessfully created server socket.");
		return true;
	}

	bool CNetworkCreator::AddSocketTcp()
	{
		int i = sdlNetWrapper_->TCPAddSocket(context_.socketSet, context_.socket);
		return i != -1;
	}

} // Network
