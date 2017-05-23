#pragma once
#include <SDL2/SDL_net.h>
#include <iostream>
#include <mutex>
#include <string>
#include <atomic>
#include <list>

const unsigned short PORT = 1234; // The port we are connecting to
const unsigned short BUFFER_SIZE = 512;  // Size of our message buffer (i.e. maximum length of characters in a message)

class SDLClientGetway
{
public:
	static SDLClientGetway& Instance();
	~SDLClientGetway();
	void Init();
	void SendMessage(const std::string& message);
	std::string GetMessage();
	bool CheckIncomingMessage();
	void CheckComplexMessage();	
	void ClearMessagesQueue();
private:
	void AddMessage(const std::string& msg);
	void RemoveMessage(std::list<std::string>::iterator& it);
	bool IsIncomingMessagesEmpty();

	SDLNet_SocketSet socketSet;
	IPaddress serverIP;       // The IP we will connect to
	TCPsocket clientSocket;   // The socket to use
	std::string   serverName;     // The server name

	bool init = false;

	std::mutex m_MessageMutex;
	std::list<std::string> m_IncomingMessages;
	std::atomic_bool recivingComplexMessage;

	SDLClientGetway() {}
};