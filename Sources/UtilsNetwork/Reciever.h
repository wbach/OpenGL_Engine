#pragma once
#include "Types.h"
#include "ISDLNetWrapper.h"
#include "SDLNetWrapper.h"
#include <memory>
#include "Logger/Log.h"

namespace Network
{
	class IMessage;

	class Receiver
	{
	public:
		Receiver(ISDLNetWrapper* sdlNetWrapper = new SDLNetWrapper());
		std::shared_ptr<IMessage> Receive(TCPsocket socket);

	private:
		template<class T>
		std::shared_ptr<IMessage> GetIMessage(TCPsocket socket)
		{
			T msg;
			auto recvBytes = sdlNetWrapper_->RecvTcp(socket, &msg, sizeof(msg));

			Log(msg.ToString());

			return std::make_shared<T>(msg);
		}

	private:
		std::shared_ptr<ISDLNetWrapper> sdlNetWrapper_;
	};
}