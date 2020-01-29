#pragma once
#include <unordered_map>
#include <Common/MessageHandling/Dispatcher.h>
#include "Scene/Scene.hpp"
#include "TestGame/MRpg/MrpgGameContext.h"

namespace Network
{
class Gateway;
}  // namespace Network

namespace MmmoRpg
{
class MRpgScene : public GameEngine::Scene
{
public:
    MRpgScene(const std::string& name, Network::Gateway& gateway, const std::string& serverAddress, MrpgGameContext& gameContext);
    virtual ~MRpgScene() override;

protected:
    std::string serverAddress_;
    Network::Gateway& gateway_;
    MrpgGameContext& gameContext_;
    common::Dispacher dispatcher_;
};
}  // namespace MmmoRpg
