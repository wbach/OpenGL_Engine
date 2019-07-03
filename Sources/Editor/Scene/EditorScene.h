#pragma once
#include "GameEngine/Scene/Scene.hpp"

namespace Editor
{
class EditorScene : public GameEngine::Scene
{
public:
    EditorScene();
    virtual ~EditorScene() override;
    virtual int Initialize();
    virtual void PostInitialize()
    {
    }
    virtual int Update();

private:
    double timeClock = 0;
};
}  // namespace Editor