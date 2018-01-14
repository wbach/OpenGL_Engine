#pragma once
#include "Types.h"
#include "Message.h"
#include "ISDLNetWrapper.h"
#include "SDLNetWrapper.h"
#include <memory>
#include "Logger/Log.h"

namespace Network
{	
	class Sender
	{
	public:
		Sender(ISDLNetWrapper* sdlNetWrapper = new SDLNetWrapper());
		bool SendTcp(TCPsocket socket, IMessage* msg);

	private:
		template <class T>
		bool SendIMessage(TCPsocket socket, IMessage* msg)
		{
			auto final_msg = dynamic_cast<T*>(msg);
			if (final_msg == nullptr)
			{
				Log("[Error] Something went wrong. Couldn't cast to : " + std::to_string(msg->GetType()));
				return false;
			}

			int sentBytes = sdlNetWrapper_->SendTcp(socket, final_msg, sizeof(T));

			if (sentBytes == 0)
				return false;

			Log("Sent message bytes : " + std::to_string(sentBytes) + "/" + std::to_string(sizeof(T)));
			Log(final_msg->ToString());
			return true;
		}

	private:
		std::shared_ptr<ISDLNetWrapper> sdlNetWrapper_;
	};
}