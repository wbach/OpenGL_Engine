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
    ~VerticalLayout() override;
    virtual LayoutElementWrapper& AddChild(GuiElement*) override;
    void SetAlgin(Algin algin);

private:
    void OnChange();
    float CalculateXPosition(const GuiElement&);
    void UpdateVisibility();

private:
    Input::InputManager& inputManager_;
    Algin algin_;
    float viewPosition_;
    uint32 mouseWheelUpSub_;
    uint32 mouseWheelDownSub_;
    float totalYScale_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
