#pragma once
#include <Types.h>
#include <functional>
#include "IMessage.h"

namespace Network
{
typedef std::pair<uint32, std::unique_ptr<IMessage>> BoxMessage;
typedef std::function<void(const BoxMessage&)> OnMessageArrived;
}  // namespace Network
