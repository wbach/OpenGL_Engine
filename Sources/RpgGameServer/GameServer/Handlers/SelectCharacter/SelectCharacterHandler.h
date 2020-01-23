#pragma once
#include "../../Context.h"
#include "Common/MessageHandling/AbstractHandler.h"
#include <Common/Messages/MessageTypes.h>

namespace GameServer
{
namespace Handler
{
class SelectCharacterHandler : public common::AbstractHandler
{
   public:
    SelectCharacterHandler(Context& context)
        : common::AbstractHandler({common::MessageTypes::SelectCharacterReq})
        , context_(context)
    {
    }

   protected:
    virtual void ProcessMessage(const Network::IMessage &message) override;

   private:
    void SendResponse(bool status, uint32 userId, uint32 characterId, uint32 mapId);

   private:
    Context& context_;
};
}  // namespace Handler
}  // namespace GameServer
