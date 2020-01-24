#pragma once
#include <Types.h>
#include <functional>
#include "IMessage.h"

namespace Network
{
typedef uint32 UserId;
typedef std::function<void(UserId, std::unique_ptr<IMessage>)> OnMessageArrived;
}  // namespace Network
