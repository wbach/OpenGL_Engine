#pragma once
#include "../User.h"
#include "Types.h"
#include "../NetworkUtils.h"
#include <SDL2/SDL_net.h>
#include <vector>
#include <map>

namespace UtlisNetwork
{
class SDLServerGetway
{
public:
	~SDLServerGetway();
	void Start(uint16 port = 1234);
	void CheckNewConnections();
	bool WaitForRespone(uint32& user_index, const std::string& response, uint32 time = 0);
	void GetMessage();
	std::string GetMessage(uint32 player_index);
	std::string GetFirstMessageToUser(uint32 index);
	void SendMessage(const UserData& user, const std::string& message);
	void SendMessage(uint32 user, const std::string& message);
	void SendMessage(std::string& message);
	void DisconnectUser(uint32 user_id);
	void ClearMessages();
	int GetUserCount() { return m_ClientsCount; }
	std::multimap<int, std::string> m_IncomingMessages;
	void DisconnectAll();
private:
	IPaddress serverIP;                  // The IP of the server (this will end up being 0.0.0.0 - which means roughly "any IP address")
	TCPsocket serverSocket;              // The server socket that clients will use to connect to us
	SDLNet_SocketSet socketSet;

	std::vector<UserData> users;

	uint32 m_MaxClients = 2;
	uint32 m_ClientsCount = 0;

	char buffer[BUFFER_SIZE];
};
} // UtilsNetwork