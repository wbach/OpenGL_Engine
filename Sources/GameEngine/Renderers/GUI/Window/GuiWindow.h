#pragma once
#include <memory>
#include <vector>
#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "Input/InputManager.h"
#include <optional>

namespace GameEngine
{
class GuiWindowElement : public GuiElement
{
public:
    GuiWindowElement(const vec2ui& windowSize, Input::InputManager& inputManager);
    void AddChild(GuiElement*);
    virtual void Update() override;

private:
    std::vector<GuiElement*> children_;
    Input::InputManager& inputManager_;
    float titleBarSize_;

public:
    static GuiElementTypes type;
    std::optional<vec2> collisionPoint_;
};
}  // namespace GameEngine
