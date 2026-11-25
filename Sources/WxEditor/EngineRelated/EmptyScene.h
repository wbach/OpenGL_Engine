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
};
}  // namespace WxEditor
