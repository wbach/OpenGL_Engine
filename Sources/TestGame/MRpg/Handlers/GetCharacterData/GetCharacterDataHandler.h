#pragma once
#include "Common/MessageHandling/AbstractHandler.h"

namespace GameEngine
{
class GameObject;
} // namespace GameEngine

namespace common
{
struct GetCharacterDataMsgResp;
}  // Network

namespace MmmoRpg
{
class NetworkCharacterManager;

class GetCharacterDataHandler : public common::AbstractHandler
{
    typedef std::function<void(uint32)> Callback;
    typedef std::function<void(GameEngine::GameObject*, const vec3&)> AddGameObject;

public:
    GetCharacterDataHandler(NetworkCharacterManager& networkCharacterManager)
        : common::AbstractHandler({common::MessageTypes::GetCharacterDataResp})
        , networkCharacterManager_(networkCharacterManager)
    {
    }

private:
    void HandleNetworkCharacterMsg(const common::GetCharacterDataMsgResp& data);

protected:
    virtual void ProcessMessage(const Network::IMessage& message) override;
    NetworkCharacterManager& networkCharacterManager_;
};
}  // MmmoRpg
