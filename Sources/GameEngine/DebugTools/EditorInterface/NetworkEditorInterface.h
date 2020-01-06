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
    void OnMessage(const Network::BoxMessage&);

private:
    Network::CGateway gateway_;
};
}  // namespace GameEngine
