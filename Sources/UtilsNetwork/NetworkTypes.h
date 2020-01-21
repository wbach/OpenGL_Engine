#pragma once
#include <Types.h>
#include <functional>
#include "IMessage.h"

namespace Network
{
typedef std::function<void(std::unique_ptr<IMessage>)> OnMessageArrived;
}  // namespace Network
