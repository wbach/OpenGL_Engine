#pragma once
#include "Common/MessageHandling/Dispatcher.h"
#include "Context.h"
#include "Gateway.h"

namespace GameServer
{
class GameServer
{
public:
    GameServer();
    void Update();

private:
    void OnMessageArrived(const Network::BoxMessage& mesage);
    void Send(uint32, Network::IMessage*);
    void ProccesSdlEvent();

private:
    bool running_;
    common::Dispacher dispatcher_;

    Network::GatewayPtr gateway_;
    Context context_;
};
}  // namespace GameServer
