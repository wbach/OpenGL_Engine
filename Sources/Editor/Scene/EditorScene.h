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
    int Initialize() override;
    void PostInitialize() override;
    int Update(float) override;

private:
    void AddGameObjects(GameEngine::GameObject*, int lvl);

private:
    Context& context_;
    double timeClock = 0;
};
}  // namespace Editor
