#include "MRpgScene.h"
#include "Gateway.h"

namespace MmmoRpg
{
MRpgScene::MRpgScene(const std::string& name, Network::CGateway& gateway, const std::string& serverAddress,
                     MrpgGameContext& gameContext)
    : GameEngine::Scene(name)
    , serverAddress_(serverAddress)
    , gateway_(gateway)
    , gameContext_(gameContext)
    , dispatcher_()
{
    gateway_.SubscribeOnMessageArrived("Dispatcher",
                                       std::bind(&common::Dispacher::Dispatch, &dispatcher_, std::placeholders::_1));
}
MRpgScene::~MRpgScene()
{
    gateway_.UnsubscribeAllOnMessageArrived();
    DEBUG_LOG("UnsubscribeAllOnMessageArrived.");
}
}  // MmmoRpg
