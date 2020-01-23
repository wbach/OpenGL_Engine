#include "GetCharacterDataHandler.h"
#include <Common/Messages/GetCharacterData/GetCharacterDataMsgResp.h>
#include <TestGame/MRpg/Characters/NetworkCharacterManger.h>

namespace MmmoRpg
{
void GetCharacterDataHandler::ProcessMessage(const Network::IMessage &message)
{
    auto msg = static_cast<const common::GetCharacterDataMsgResp*>(&message);

    if (msg == nullptr)
    {
        ERROR_LOG("SelectCharacterScene::WaitForGetCharacterResp, got msg but wrong type : " +
            std::to_string(msg->GetType()));
        return;
    }
    HandleNetworkCharacterMsg(*msg);
}

void GetCharacterDataHandler::HandleNetworkCharacterMsg(const common::GetCharacterDataMsgResp& data)
{
    networkCharacterManager_.AddCharacter(data.networkCharcterId, data.characterData.classId, data.position,
                                          data.rotation, data.commonStats);
}

}  // MmmoRpg
