#pragma once
#include "Layout.h"

namespace GameEngine
{
class HorizontalLayout : public Layout
{
public:
    HorizontalLayout(const vec2ui& windowSize, Input::InputManager& inputManager);
    virtual void Update() override;

private:
    float CalculateXPosition(const GuiElement&);
    void OnChange() override;
    void UpdateVisibility();

private:
    Input::InputManager& inputManager_;
    float totalChildrenScale_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
