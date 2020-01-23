#pragma once
#include "../../Context.h"
#include "Common/MessageHandling/AbstractHandler.h"

namespace GameServer
{
namespace Handler
{
class DistributeHandler : public common::AbstractHandler
{
   public:
    DistributeHandler(Context& context)
        : common::AbstractHandler({Network::MessageTypes::Any})
        , context_(context)
    {
    }

   protected:
    virtual void ProcessMessage(const Network::IMessage& message) override;

   private:
    Context& context_;
};
}  // namespace Handler
}  // namespace GameServer
