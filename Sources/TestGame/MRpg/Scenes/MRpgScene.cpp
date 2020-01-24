#include "MRpgScene.h"
#include <Common/Messages/MessageTypes.h>
#include <UtilsNetwork/Gateway.h>

namespace MmmoRpg
{
MRpgScene::MRpgScene(const std::string& name, Network::Gateway& gateway, const std::string& serverAddress, MrpgGameContext& gameContext)
    : GameEngine::Scene(name)
    , serverAddress_(serverAddress)
    , gateway_(gateway)
    , gameContext_(gameContext)
    , dispatcher_()
{
    gateway_.SubscribeOnMessageArrived(common::MessageTypes::Any, [&](auto userId, auto message) { dispatcher_.Dispatch(userId, *message); });
}
MRpgScene::~MRpgScene()
{
    gateway_.UnsubscribeAllOnMessageArrived();
    DEBUG_LOG("UnsubscribeAllOnMessageArrived.");
}
}  // namespace MmmoRpg
