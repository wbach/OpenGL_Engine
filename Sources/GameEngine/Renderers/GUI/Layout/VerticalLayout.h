#pragma once
#include "Layout.h"

namespace GameEngine
{
class VerticalLayout : public Layout
{
public:
    VerticalLayout(const vec2ui& windowSize, Input::InputManager& inputManager, std::function<void(uint32)> unsubscribe);
    virtual LayoutElementWrapper& AddChild(GuiElement*) override;

private:
    void OnChange();

private:
    Input::InputManager& inputManager_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
