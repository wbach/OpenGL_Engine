#pragma once
#include <functional>

#include "GameEngine/Scene/Scene.hpp"

namespace AvatarGame
{
class PauseMenu;

class MainMenu : public GameEngine::Scene
{
public:
    MainMenu();
    virtual ~MainMenu() override;

    virtual int Initialize() override;
    virtual void PostInitialize() override;
    virtual int Update(float) override;

private:
    void prepareMenu();

private:
    std::unique_ptr<PauseMenu> menu_;
};
}  // namespace AvatarGame
