#include "DistributeHandler.h"
#include <UtilsNetwork/MessageTarget.h>

namespace GameServer
{
namespace Handler
{
void DistributeHandler::ProcessMessage(Network::UserId userId, const Network::IMessage &message)
{
    switch (message.GetTarget())
    {
        case Network::MessageTarget::All:
            for (uint32 x = 0; x < context_.GetUsers().size(); ++x)
                context_.sendMessage_(userId, message);
            break;
    }
}

}  // namespace Handler
}  // namespace GameServer
