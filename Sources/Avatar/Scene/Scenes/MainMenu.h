#pragma once
#include <functional>
#include "GameEngine/Scene/Scene.hpp"

namespace AvatarGame
{
class MainMenu : public GameEngine::Scene
{
public:
    MainMenu();
    virtual ~MainMenu() override;

    virtual int Initialize() override;
    virtual void PostInitialize() override;
    virtual int Update(float) override;
};
}  // namespace AvatarGame
