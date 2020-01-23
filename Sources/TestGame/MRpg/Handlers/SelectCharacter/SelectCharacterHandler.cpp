#include "SelectCharacterHandler.h"
#include <Common/Messages/SelectCharacter/SelectCharacterMsgResp.h>

namespace MmmoRpg
{
void SelectCharacterHandler::ProcessMessage(const Network::IMessage &message)
{
    auto msg = static_cast<const common::SelectCharacterMsgResp*>(&message);

    if (msg == nullptr)
    {
        ERROR_LOG("SelectCharacterScene::WaitForGetCharacterResp, got msg but wrong type : " + std::to_string(message.GetType()));
        return;
    }

    callback_(msg->id, msg->id, msg->status_ == common::MessageStatus::Ok);
}

}  // namespace MmmoRpg
