#pragma once
#include "Layout.h"

namespace GameEngine
{
class VerticalLayout : public Layout
{
public:
    VerticalLayout(const vec2ui& windowSize, Input::InputManager& inputManager);
    virtual LayoutElementWrapper& AddChild(GuiElement*) override;
    virtual void SetScale(const vec2& scale);
    virtual void Show(bool);
    virtual void Show() override;
    virtual void Hide() override;

private:
    void OnChange();

private:
    Input::InputManager& inputManager_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
