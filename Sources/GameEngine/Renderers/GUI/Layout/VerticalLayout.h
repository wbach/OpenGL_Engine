#pragma once
#include "Layout.h"

namespace GameEngine
{
class VerticalLayout : public Layout
{
public:
    enum class Algin
    {
        LEFT,
        CENTER,
        RIGHT
    };

    VerticalLayout(const vec2ui& windowSize, Input::InputManager& inputManager, std::function<void(uint32)> unsubscribe);
    virtual LayoutElementWrapper& AddChild(GuiElement*) override;
    void SetAlgin(Algin algin);

private:
    void OnChange();

private:
    Input::InputManager& inputManager_;
    Algin algin_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
