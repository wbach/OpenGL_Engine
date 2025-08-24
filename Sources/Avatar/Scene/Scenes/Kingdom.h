#pragma once
#include "GameEngine/Scene/Scene.hpp"

class PauseMenu;

namespace AvatarGame
{
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
    std::optional<IdType> cameraId_;
};
}  // namespace AvatarGame
