#pragma once
#include "GameEngine/Scene/Scene.hpp"

namespace Editor
{
class GuiEditScene : public GameEngine::Scene
{
public:
    GuiEditScene();
    virtual ~GuiEditScene() override;
    virtual int Initialize();
    virtual void PostInitialize()
    {
    }
    virtual int Update();

private:
    double timeClock = 0;
};
}  // namespace Editor