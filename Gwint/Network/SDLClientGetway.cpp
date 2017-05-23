#include "SDLClientGetway.h"
#include "NetworkUtils.h"
#include <Utils/Types.h>
#include <Debug_/Log.h>
#include <Utils/Utils.h>
#include <algorithm>

SDLClientGetway & SDLClientGetway::Instance()
{
	static SDLClientGetway getway;
	return getway;
}

SDLClientGetway::~SDLClientGetway()
{
	if (!init)
		return;

	SDLNet_FreeSocketSet(socketSet);
	SDLNet_Quit();
}

void SDLClientGetway::Init()
{
	// Initialise SDL_net

	std::string userInput = "";    // A string to hold our user input
	int inputLength = 0;     // The length of our string in characters
	char buffer[BUFFER_SIZE]; // Array of character's we'll use to transmit our message. We get input into the userInput string for ease of use, then just copy it to this character array and send it.

	ZERO_MEM(buffer);
	//memset(buffer, 0, BUFFER_SIZE);

	if (SDLNet_Init() < 0)
	{
		Error("Failed to intialise SDN_net: " + std::string(SDLNet_GetError()));
		exit(-1); // Quit!
	}
	socketSet = SDLNet_AllocSocketSet(1);

	serverName = Utils::ReadFile("../Data/server.conf");	

	Log("Server : " + serverName);

	int hostResolved = SDLNet_ResolveHost(&serverIP, serverName.c_str(), PORT);
	const char* host = SDLNet_ResolveIP(&serverIP);
	clientSocket = SDLNet_TCP_Open(&serverIP);

	SDLNet_TCP_AddSocket(socketSet, clientSocket);
	int activeSockets = SDLNet_CheckSockets(socketSet, 5000);
	int gotServerResponse = SDLNet_SocketReady(clientSocket);

	bool is_connected = false;

	if (gotServerResponse != 0)
	{
		//cout << "Got a response from the server... " << endl;
		int serverResponseByteCount = SDLNet_TCP_Recv(clientSocket, buffer, BUFFER_SIZE);

		//cout << "Got the following from server: " << buffer << "(" << serverResponseByteCount << " bytes)" << endl;

		std::string buff_string(buffer);

		// We got an okay from the server, so we can join!
		if (buff_string == "OK")
		{
			is_connected = true;
			// So set the flag to say we're not quitting out just yet
			//shutdownClient = false;

			std::cout << "Joining server now..." << std::endl << std::endl;
		}
		else
		{
			//cout << "Server is full... Terminating connection." << endl;
		}
	}
	else
	{
		//cout << "No response from server..." << endl;
	}
	init = true;
}

void SDLClientGetway::SendMessage(const std::string & message)
{
	NetworkUtils::SendMessage(clientSocket, message);
}

std::string SDLClientGetway::GetMessage()
{
	if (recivingComplexMessage)
		return std::string();

	std::lock_guard<std::mutex> lock(m_MessageMutex);
	if (!m_IncomingMessages.empty())
	{
		auto str = m_IncomingMessages.front();
		m_IncomingMessages.pop_front();
		return str;
	}		
	else
		return std::string();
}

bool SDLClientGetway::CheckIncomingMessage()
{
	int socketActive = SDLNet_CheckSockets(socketSet, 0);

	if (socketActive == 0)
		return false;

	std::lock_guard<std::mutex> lock(m_MessageMutex);

	bool start_complex = false;
	auto recv = NetworkUtils::CheckIncomingMessage(m_IncomingMessages, clientSocket, start_complex);

	if (start_complex)
		recivingComplexMessage = true;
	return recv > 0;
}

void SDLClientGetway::CheckComplexMessage()
{
	std::lock_guard<std::mutex> lock(m_MessageMutex);

	if (NetworkUtils::CheckComplexMessage(m_IncomingMessages))
		recivingComplexMessage = false;
}

void SDLClientGetway::ClearMessagesQueue()
{
	std::lock_guard<std::mutex> lock(m_MessageMutex);
	m_IncomingMessages.clear();
}

void SDLClientGetway::AddMessage(const std::string & msg)
{
	if (msg.empty())
		return;

	if (msg == "START_SEND_PARTS")
		recivingComplexMessage = true;

	Log("SDLGetway add incoming message : " + msg);

	std::lock_guard<std::mutex> lock(m_MessageMutex);
	m_IncomingMessages.push_back(msg);
}

void SDLClientGetway::RemoveMessage(std::list<std::string>::iterator & it)
{
	std::lock_guard<std::mutex> lock(m_MessageMutex);
	m_IncomingMessages.erase(it);
}

bool SDLClientGetway::IsIncomingMessagesEmpty()
{
	std::lock_guard<std::mutex> lock(m_MessageMutex);
	return m_IncomingMessages.empty();
}
