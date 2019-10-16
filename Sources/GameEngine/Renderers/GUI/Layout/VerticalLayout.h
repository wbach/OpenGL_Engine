#pragma once
#include "Layout.h"

namespace GameEngine
{
class VerticalLayout : public Layout
{
public:
    VerticalLayout(const vec2ui& windowSize, Input::InputManager& inputManager);
    ~VerticalLayout() override;
    virtual void ResetView() override;

private:
    void OnChange() override;
    float CalculateXPosition(const GuiElement&);
    void UpdateVisibility();
    void EnableScroll();
    void DisableScroll();

private:
    Input::InputManager& inputManager_;
    float viewPosition_;
    std::optional<uint32> mouseWheelUpSub_;
    std::optional<uint32> mouseWheelDownSub_;
    float scrollSensitive_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
