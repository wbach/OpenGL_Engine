#pragma once
#include "../../Context.h"
#include "Common/MessageHandling/AbstractHandler.h"
#include <Common/Messages/MessageTypes.h>

namespace GameServer
{
namespace Handler
{
class GetCharactersDataHandler : public common::AbstractHandler
{
public:
    GetCharactersDataHandler(Context& context)
        : common::AbstractHandler(
              {common::MessageTypes::GetCharactersDataReq, common::MessageTypes::GetCharacterDataReq})
        , context_(context)
    {
    }

protected:
    virtual void ProcessMessage(const Network::IMessage& message) override;
    void ProccesGetCharactersDataReq(const Network::IMessage& message);
    void ProccesGetCharacterDataReq(const Network::IMessage& message);

private:
    Context& context_;
};
}  // Handler
}  // GameServer
