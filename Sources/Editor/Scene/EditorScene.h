#pragma once
#include "GameEngine/Scene/Scene.hpp"

namespace Editor
{
struct Context;

class EditorScene : public GameEngine::Scene
{
public:
    EditorScene(Context& context);
    virtual ~EditorScene() override;
    virtual int Initialize();
    virtual void PostInitialize()
    {
    }
    virtual int Update();

private:
    Context& context_;
    double timeClock = 0;
};
}  // namespace Editor