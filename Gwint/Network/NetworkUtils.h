#pragma once
#include <list>
#include <string>

typedef struct _TCPsocket *TCPsocket;

namespace NetworkUtils
{
	const unsigned short BUFFER_SIZE = 512;
	bool CheckComplexMessage(std::list<std::string>& incoming_messages);
	void SendSingleMessage(TCPsocket socket, const std::string & message);
	void SendMessage(TCPsocket socket, const std::string & message);
	int CheckIncomingMessage(std::list<std::string>& incoming_messages, TCPsocket socket, bool& complex_msg_recv);
}