#pragma once
#include "GameEngine/Scene/Scene.hpp"

namespace AvatarGame
{
class SouthPole : public GameEngine::Scene
{
public:
    SouthPole();
    virtual ~SouthPole() override;

    virtual int Initialize() override;
    virtual void PostInitialize() override;
    virtual int Update(float) override;

private:
    void prepareMenu();
};
}  // namespace AvatarGame
