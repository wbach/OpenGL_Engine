#pragma once
#include "Layout.h"

namespace GameEngine
{
class HorizontalLayout : public Layout
{
public:
    HorizontalLayout(const vec2ui& windowSize, Input::InputManager& inputManager);
    virtual LayoutElementWrapper& AddChild(std::unique_ptr<GuiElement>) override;

private:
    float CalculateXPosition(const GuiElement&);
    void OnChange();

private:
    Input::InputManager& inputManager_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
