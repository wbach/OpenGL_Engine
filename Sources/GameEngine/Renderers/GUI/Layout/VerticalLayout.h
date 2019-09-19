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

    VerticalLayout(const vec2ui& windowSize, Input::InputManager& inputManager);
    ~VerticalLayout() override;
    virtual void ResetView() override;
    virtual LayoutElementWrapper& AddChild(std::unique_ptr<GuiElement>) override;
    void SetAlgin(Algin algin);
    virtual void SetPostion(const vec2& position) override;
    virtual void SetPostion(const vec2ui& position) override;

private:
    void OnChange();
    float CalculateXPosition(const GuiElement&);
    void UpdateVisibility();
    void EnableScroll();
    void DisableScroll();

private:
    Input::InputManager& inputManager_;
    Algin algin_;
    float viewPosition_;
    std::optional<uint32> mouseWheelUpSub_;
    std::optional<uint32> mouseWheelDownSub_;
    float scrollSensitive_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
