#pragma once
#include "GameEngine/Scene/Scene.hpp"

namespace AvatarGame
{
class PauseMenu;

class GreenIsland : public GameEngine::Scene
{
public:
    GreenIsland();
    virtual ~GreenIsland() override;

    virtual int Initialize() override;
    virtual void PostInitialize() override;
    virtual int Update(float) override;

private:
    void prepareMenu();
    void keyOperations();

private:
    std::unique_ptr<PauseMenu> menu_;
};
}  // namespace AvatarGame
