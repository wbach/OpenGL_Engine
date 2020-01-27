#pragma once
#include <UtilsNetwork/Gateway.h>
#include "IEditorInterface.hpp"
#include <unordered_map>
#include <vector>
#include <functional>

namespace GameEngine
{
class NetworkEditorInterface : public IEditorInterface
{
public:
    NetworkEditorInterface();
    ~NetworkEditorInterface();
    virtual void AddObject(const std::string&) override;

private:
    void NewUser(const std::string&, uint32);
    void DisconnectUser(uint32);
    void OnMessage(Network::UserId userId, std::unique_ptr<Network::IMessage>);

private:
    Network::Gateway gateway_;
    std::thread networkThread_;
    std::atomic_bool isRunning_;
    std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> commands_;
};
}  // namespace GameEngine
