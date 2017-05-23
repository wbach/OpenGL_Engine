#include "NetworkUtils.h"
#include <Debug_/Log.h>
#include <Utils/Types.h>
#include <SDL2/SDL_net.h>

bool NetworkUtils::CheckComplexMessage(std::list<std::string>& incoming_messages)
{
	if (incoming_messages.empty())
		return false;

	std::list<std::list<std::string>::iterator> elements_to_remove;

	auto start = std::find(std::begin(incoming_messages), std::end(incoming_messages), "START_SEND_PARTS");
	auto end = std::find(std::begin(incoming_messages), std::end(incoming_messages), "END_SEND_PARTS");

	std::string tmp_msg;

	if (start == std::end(incoming_messages) || end == std::end(incoming_messages))
		return false;

	if ((++start) == std::end(incoming_messages))
		return false;

	for (auto it = start; it != end; ++it)
	{
		tmp_msg += *it;
		elements_to_remove.push_back(it);
	}
	//to do :  don clear all message list!
	incoming_messages.clear();

	/*incoming_messages.erase(start);
	incoming_messages.erase(end);

	for (auto& it : elements_to_remove)
	{
	it = incoming_messages.erase(it);
	}*/

	if (!tmp_msg.empty())
	{
		Log("###################################################");
		Log("Add complex messaeg  : " + tmp_msg);
		incoming_messages.push_back(tmp_msg);
		Log("###################################################");
	}
	return true;
}

void NetworkUtils::SendSingleMessage(TCPsocket socket, const std::string & message)
{
	if (message.empty())
	{
		Log("Try sent empty message");
		return;
	}
	char buffer[BUFFER_SIZE];
	ZERO_MEM(buffer);

	for (uint x = 0; x < message.size(); ++x)
		buffer[x] = message[x];

	auto sended_bytes = SDLNet_TCP_Send(socket, buffer, BUFFER_SIZE);
	Log("Sended message, size : " + std::to_string(message.size()) + " " + std::to_string(sended_bytes) + "/" + std::to_string(BUFFER_SIZE) + " (" + std::to_string((int)(((float)sended_bytes / (float)(BUFFER_SIZE))*100.f)) + "%)");
	Log("Sended : \n" + message);
}

void NetworkUtils::SendMessage(TCPsocket socket, const std::string & message)
{
	auto msize = message.size();
	uint max_frame_size = BUFFER_SIZE - 1;
	if (msize > max_frame_size)
	{
		auto c_message = message;
		std::list<std::string> parts;
		while (1)
		{
			auto delta = msize - max_frame_size;
			if (c_message.size() > max_frame_size)
			{
				std::string part = c_message.substr(0, max_frame_size);
				parts.push_back(part);
				c_message = c_message.substr(max_frame_size);
			}
			else
			{
				parts.push_back(c_message);
				break;
			}
		}
		//std::cout << "Sending message which size is over "+ std::to_string(max_frame_size) + ", size : " << message.size() << std::endl;
		std::string start_msg("START_SEND_PARTS");
		SendSingleMessage(socket, start_msg);
		for (const auto& part : parts)
		{
			//std::cout << "Sending part, size : " << part.size() << std::endl;
			SendSingleMessage(socket, part);
		}
		std::string end_msg("END_SEND_PARTS");
		SendSingleMessage(socket, end_msg);
		//std::cout << "End sending message which size is over "+ std::to_string(max_frame_size) +", size. " << std::endl;
	}
	else
	{
		SendSingleMessage(socket, message);
	}
}

int NetworkUtils::CheckIncomingMessage(std::list<std::string>& incoming_messages, TCPsocket socket, bool& complex_msg_recv)
{
	complex_msg_recv = false;

	int messageFromServer = SDLNet_SocketReady(socket);

	if (messageFromServer == 0)
		return -2;

	char buffer[BUFFER_SIZE];
	ZERO_MEM(buffer);
	int recv_bytes = SDLNet_TCP_Recv(socket, buffer, BUFFER_SIZE);

	if (recv_bytes <= 0)
		return recv_bytes;

	std::string message;
	for (int x = 0; x < recv_bytes; x++)
	{
		if (buffer[x] == '\0')
			continue;

			/*{
			if (!message.empty())
			{
				if (message == "START_SEND_PARTS")
					complex_msg_recv = true;

				incoming_messages.push_back(message);
				message.clear();
			}
		}
		else
		{*/
			message += buffer[x];
		//}
	}
	if (!message.empty())
	{
		if (message == "START_SEND_PARTS")
			complex_msg_recv = true;

		incoming_messages.push_back(message);
	}
	return recv_bytes;
}
