#pragma once
#include "GameEngine/Scene/Scene.hpp"

class PauseMenu;

namespace AvatarGame
{
class SouthPole : public GameEngine::Scene
{
public:
    SouthPole();
    ~SouthPole() override;

    int Initialize() override;
    void PostInitialize() override;
    int Update(float) override;

private:
    void prepareMenu();
    void keyOperations();

private:
    std::unique_ptr<PauseMenu> menu_;
};
}  // namespace AvatarGame
