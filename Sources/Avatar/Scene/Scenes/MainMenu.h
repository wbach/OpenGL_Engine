#pragma once
#include <functional>
#include "GameEngine/Scene/Scene.hpp"

namespace AvatarGame
{
enum class State
{
    MAIN,
    LOAD_GAME,
    SETTINGS
};

class MainMenu : public GameEngine::Scene
{
public:
    MainMenu();
    virtual ~MainMenu() override;

    virtual int Initialize() override;
    virtual void PostInitialize() override;
    virtual int Update(float) override;
    
private:
    void ChangeState(State);

private:
    std::optional<State> stateToChange_;
};
}  // namespace AvatarGame
