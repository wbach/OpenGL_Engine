#pragma once
#include "Types.h"
#include <list>
#include <string>
#include <SDL2/SDL_net.h>

typedef struct _TCPsocket *TCPsocket;

namespace UtlisNetwork
{
	const uint16 BUFFER_SIZE = 512;
	const uint16 PORT = 1234; // The port we are connecting to
	
	bool CheckComplexMessage(std::list<std::string>& incoming_messages);
	void SendSingleMessage(TCPsocket socket, const std::string & message);
	void SendMessage(TCPsocket socket, const std::string & message);
	int CheckIncomingMessage(std::list<std::string>& incoming_messages, TCPsocket socket, bool& complex_msg_recv);
} // UtlisNetwork