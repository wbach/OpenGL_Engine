#pragma once
#include <GameEngine/Scene/Scene.hpp>

namespace WxEditor
{
class EmptyScene : public GameEngine::Scene
{
public:
    EmptyScene()
        : Scene("Empty")
    {
    }
    int Initialize() override
    {
        return 0;
    }
    void PostInitialize() override
    {
    }
    int Update(float) override
    {
        return 0;
    }
};
}  // namespace WxEditor
