#pragma once
#include "Types.h"
#include "MessageTypes.h"
#include <memory>
#include <cstring>

namespace Network
{
	const uint16 arraySize = 256;

	static void CopyToArray(char* dest, const std::string& str)
	{
		int i = 0;
		for (auto c : str)
		{
			if (i >= arraySize - 1)
				break;

			dest[i] = c;
			++i;
		}
	}

	struct MessageHeader
	{
		uint8  msgType;
	};

	class IMessage
	{
	public:
		virtual ~IMessage() {}
		virtual std::string ToString() { return std::string(); }
		IMessage(uint8 type, uint8 target)
			: msgType(type)
			, target(target)
		{}
		uint8 GetType() { return msgType; }
		uint8 GetTarget() { return target; }
	private:
		uint8 msgType;
		uint8 target;
	};

	typedef std::shared_ptr<IMessage> IMessagePtr;
	
	template<class T>
	static IMessagePtr CreateIMessagePtr(const T& msg)
	{
		return std::make_shared<T>(msg);
	}
}
