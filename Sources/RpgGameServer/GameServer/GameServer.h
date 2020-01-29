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
    void OnMessageArrived(uint32 userId, std::unique_ptr<Network::IMessage> mesage);
    void Send(uint32, const Network::IMessage &message);
    void ProccesSdlEvent();

private:
    bool running_;
    common::Dispacher dispatcher_;

    Network::Gateway gateway_;
    Context context_;
};
}  // namespace GameServer
