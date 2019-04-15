#pragma once
#include "Types.h"
#include "Messages/IMessage.h"
#include "ISDLNetWrapper.h"
#include "SDLNetWrapper.h"
#include <memory>
#include "Logger/Log.h"

namespace Utils
{
	namespace Time
	{
		class CTimeMeasurer;
	} // Time
} // Utils

namespace Network
{	
	enum class SentStatus
	{
		OK,
		ERROR,
		EMPTY,
		CAST_ERROR
	};
	class Sender
	{
	public:
		Sender(Utils::Time::CTimeMeasurer&, ISDLNetWrapperPtr sdlNetWrapper);
		SentStatus SendTcp(TCPsocket socket, IMessage* msg);

	private:
		void PrintSentBytesPerSec();

		template <class T>
		SentStatus SendIMessage(TCPsocket socket, IMessage* msg)
		{
			auto final_msg = castMessageAs<T>(msg);
			if (final_msg == nullptr)
			{
				ERROR_LOG("Something went wrong. Couldn't cast to : " + std::to_string(msg->GetType()));
				return SentStatus::CAST_ERROR;
			}

			int length = sizeof(T);
			int sentBytes = sdlNetWrapper_->SendTcp(socket, final_msg, sizeof(T));

			if (sentBytes < length)
				return SentStatus::ERROR;

			sentBytes_ += sentBytes;

			//DEBUG_LOG("Sent message bytes : " + std::to_string(sentBytes) + "/" + std::to_string(sizeof(T)));
			//DEBUG_LOG(final_msg->ToString());
			return SentStatus::OK;
		}

	private:
		std::shared_ptr<ISDLNetWrapper> sdlNetWrapper_;
		Utils::Time::CTimeMeasurer& timer_;
		uint32 sentBytes_;
	};
}