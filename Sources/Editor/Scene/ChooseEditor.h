#pragma once
#include "GameEngine/Scene/Scene.hpp"

namespace Editor
{
class ChooseEditor : public GameEngine::Scene
{
public:
    ChooseEditor();
    virtual ~ChooseEditor() override;
    virtual int Initialize();
    virtual void PostInitialize()
    {
    }
    virtual int Update();

private:
    double timeClock = 0;
};
}  // namespace Editor