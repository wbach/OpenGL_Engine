#pragma once
#include "Types.h"
#include "ISDLNetWrapper.h"
#include "SDLNetWrapper.h"
#include <memory>
#include "Logger/Log.h"

namespace Network
{
	class IMessage;

	enum class RecvError
	{
		None,
		Disconnect,
		ZeroBytes
	};

	class Receiver
	{
	public:
		Receiver(ISDLNetWrapperPtr sdlNetWrapper);
		std::shared_ptr<IMessage> Receive(TCPsocket socket, RecvError& error);

	private:
		template<class T>
		std::shared_ptr<IMessage> GetIMessage(TCPsocket socket)
		{
			T msg;
			auto recvBytes = sdlNetWrapper_->RecvTcp(socket, &msg, sizeof(msg));

			Log("Recv bytes: " + std::to_string(recvBytes) + "Message:\n" + msg.ToString());

			return std::make_shared<T>(msg);
		}

	private:
		ISDLNetWrapperPtr sdlNetWrapper_;
	};
}