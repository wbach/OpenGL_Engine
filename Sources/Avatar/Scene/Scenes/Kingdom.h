#pragma once
#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
class FirstPersonCamera;
}

namespace AvatarGame
{
class PauseMenu;

class Kingdom : public GameEngine::Scene
{
public:
    Kingdom();
    virtual ~Kingdom() override;

    virtual int Initialize() override;
    virtual void PostInitialize() override;
    virtual int Update(float) override;

private:
    void prepareMenu();
    void keyOperations();

private:
    std::unique_ptr<PauseMenu> menu_;
    std::unique_ptr<GameEngine::FirstPersonCamera> camera_;
};
}  // namespace AvatarGame
