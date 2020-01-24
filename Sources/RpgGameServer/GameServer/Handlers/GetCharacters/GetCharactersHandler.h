#pragma once
#include "../../Context.h"
#include "Common/MessageHandling/AbstractHandler.h"
#include <Common/Messages/MessageTypes.h>

namespace GameServer
{
namespace Handler
{
class GetCharactersHandler : public common::AbstractHandler
{
public:
    GetCharactersHandler(Context& context)
        : common::AbstractHandler({common::MessageTypes::GetCharactersReq})
        , context_(context)
    {
    }

protected:
    virtual void ProcessMessage(Network::UserId userId, const Network::IMessage &message) override;

private:
    Context& context_;
};
}  // Handler
}  // GameServer
