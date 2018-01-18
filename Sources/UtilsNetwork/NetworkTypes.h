#pragma once
#include "Types.h"
#include "Message.h"
#include <functional>

namespace Network
{
	typedef std::pair<uint32, IMessagePtr> BoxMessage;
	typedef std::function<void(const BoxMessage&)> OnMessageArrived;
}