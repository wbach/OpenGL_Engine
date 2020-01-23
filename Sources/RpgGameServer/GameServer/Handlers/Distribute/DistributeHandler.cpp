#include "DistributeHandler.h"
#include <UtilsNetwork/MessageTarget.h>

namespace GameServer
{
namespace Handler
{
void DistributeHandler::ProcessMessage(const Network::IMessage &message)
{
    if (!message.second)
        return;

    switch (message.second->GetTarget())
    {
        case Network::MessageTarget::All:
            for (uint32 x = 0; x < context_.GetUsers().size(); ++x)
                context_.sendMessage_(message.first, message.second.get());
            break;
    }
}

}  // namespace Handler
}  // namespace GameServer
