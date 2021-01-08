#pragma once
#include "GameEngine/Scene/Scene.hpp"

namespace AvatarGame
{
class SouthPool : public GameEngine::Scene
{
public:
    SouthPool();
    virtual ~SouthPool() override;

    virtual int Initialize() override;
    virtual void PostInitialize() override;
    virtual int Update(float) override;

private:
    void prepareMenu();
};
}  // namespace AvatarGame
