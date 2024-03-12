#pragma once
#include "GameEngine/Scene/Scene.hpp"

namespace Editor
{
class ChooseEditor : public GameEngine::Scene
{
public:
    ChooseEditor();
    int Initialize() override;
};
}  // namespace Editor
