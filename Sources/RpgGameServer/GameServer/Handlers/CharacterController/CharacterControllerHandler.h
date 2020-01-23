#pragma once
#include <Common/Messages/MessageTypes.h>
#include "../../Context.h"
#include "Common/MessageHandling/AbstractHandler.h"

namespace GameServer
{
namespace Handler
{
class CharacterControllerHandler : public common::AbstractHandler
{
   public:
    CharacterControllerHandler(Context& context);

   protected:
    virtual void ProcessMessage(const Network::IMessage& message) override;

   private:
    Context& context_;
};
}  // namespace Handler
}  // namespace GameServer
