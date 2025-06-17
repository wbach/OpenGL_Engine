#pragma once
#include "GameEngine/Scene/Scene.hpp"

namespace AvatarGame
{
class PauseMenu;

class Gorzow : public GameEngine::Scene
{
public:
    Gorzow();
    ~Gorzow() override;

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
