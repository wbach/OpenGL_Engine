#pragma once
#include <memory>
#include <vector>
#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "Input/InputManager.h"

namespace GameEngine
{
class GuiWindowElement : public GuiElement
{
public:
    GuiWindowElement(const vec2ui& windowSize, Input::InputManager& inputManager);
    void AddChild(std::unique_ptr<GuiElement>);
    virtual void Update() override;

private:
    std::vector<std::unique_ptr<GuiElement>> children_;
    Input::InputManager& inputManager_;
    float titleBarSize_;

public:
    static GuiElementTypes type;
    std::optional<vec2> collisionPoint_;
};
}  // namespace GameEngine
