#pragma once
#include "Types.h"
#include "ISDLNetWrapper.h"
#include "SDLNetWrapper.h"
#include <memory>
#include "Logger/Log.h"

namespace Utils
{
	namespace Time
	{
		class CTimeMeasurer;
	}
} // Utils

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
		Receiver(Utils::Time::CTimeMeasurer&, ISDLNetWrapperPtr sdlNetWrapper);
		std::shared_ptr<IMessage> Receive(TCPsocket socket, RecvError& error);
	
	private:
		void PrintRecvBytesPerSec();

		template<class T>
		std::shared_ptr<IMessage> GetIMessage(TCPsocket socket, RecvError& error)
		{
			T msg;
			auto recvBytes = sdlNetWrapper_->RecvTcp(socket, &msg, sizeof(msg));

			if (recvBytes <= 0)
			{
				Log("Recv header bytes : -1, Disconnect.");
				error = RecvError::Disconnect;
				return nullptr;
			}

			//Log("Recv bytes: " + std::to_string(recvBytes) + "Message:\n" + msg.ToString());
			recvBytes_ += recvBytes;
			return std::make_shared<T>(msg);
		}

	private:
		ISDLNetWrapperPtr sdlNetWrapper_;
		Utils::Time::CTimeMeasurer& timer_;
		uint32 recvBytes_;
	};
}