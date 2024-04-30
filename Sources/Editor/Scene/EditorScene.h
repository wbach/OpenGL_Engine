#pragma once
#include "GameEngine/Scene/Scene.hpp"

namespace Editor
{
struct Context;

class EditorScene : public GameEngine::Scene
{
public:
    EditorScene();
    ~EditorScene() override;
    int Initialize() override;
    void PostInitialize() override;
    int Update(float) override;

private:
    void AddGameObjects(GameEngine::GameObject*, int lvl);
    void KeySubscribtions();

private:
    GameEngine::GameObject* gameObject;
    std::vector<uint32> clipsButtonsIds_;
};
}  // namespace Editor
