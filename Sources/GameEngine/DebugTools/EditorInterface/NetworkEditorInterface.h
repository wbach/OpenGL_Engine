#pragma once
#include "IEditorInterface.hpp"
#include <UtilsNetwork/Gateway.h>

namespace GameEngine
{
class NetworkEditorInterface : public IEditorInterface
{
public:
    NetworkEditorInterface();
    virtual void AddObject(const std::string&) override;

private:
    void NewUser(const std::string&, uint32);
    void DisconnectUser(uint32);
    void OnMessage(Network::UserId userId, std::unique_ptr<Network::IMessage>);

private:
    Network::Gateway gateway_;
};
}  // namespace GameEngine
