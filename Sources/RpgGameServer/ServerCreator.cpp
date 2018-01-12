#include "ServerCreator.h"
#include "Logger/Log.h"
#include "NetworkUtils.h"

namespace Network
{
	SServerConectContext CNetworkCreator::Create(uint32 maxClients, uint32 port)
	{
		context_.maxClients = maxClients;
		context_.port		= port;

		Init();
		AllocSocketSet();
		ResolveHost();
		OpenTcp();
		SDLNet_TCP_AddSocket(context_.socketSet, context_.serverSocket);
		Log("Connection is open.");

		return context_;
	}

	void CNetworkCreator::Init()
	{
		if (SDLNet_Init() != -1)
			return;

		Error("Failed to intialise SDL_net: " + std::string(SDLNet_GetError()));
		exit(-1);
	}

	void CNetworkCreator::AllocSocketSet()
	{
		context_.socketSet = SDLNet_AllocSocketSet(context_.maxClients + 1);

		if (context_.socketSet == NULL)
		{
			Error("Failed to allocate the socket set:: " + std::string(SDLNet_GetError()));
			exit(-1); // Quit!
		}
		
		std::string str = "Allocated socket set with size:  " + std::to_string(context_.maxClients + 1) + ", of which " + std::to_string(context_.maxClients) + " are availble for use by clients.";
		Log(str);		
	}

	void CNetworkCreator::ResolveHost()
	{
		int hostResolved = SDLNet_ResolveHost(&context_.serverIP, NULL, context_.port);

		if (hostResolved == -1)
		{
			Error("Failed to resolve the server host: " + std::string(SDLNet_GetError()));
			exit(1);
		}
		else // If we resolved the host successfully, output the details
		{
			Log("Successfully resolved server host to IP: " + UtlisNetwork::IpAddressToString(context_.serverIP));
		}
	}

	void CNetworkCreator::OpenTcp()
	{
		// Try to open the server socket
		context_.serverSocket = SDLNet_TCP_Open(&context_.serverIP);

		if (!context_.serverSocket)
		{
			Error("Failed to open the server socket: " + std::string(SDLNet_GetError()));
			exit(-1);
		}

		Log("Sucessfully created server socket.");		
	}

} // Network
