#pragma once
#include "Message.h"

namespace GameServer
{
class AbstractUser
{
   public:
    virtual void MessageArrived(common::TransformMsg msg)
    {
    }
};
}  // namespace GameServer
