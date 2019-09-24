#pragma once
#include "Layout.h"

namespace GameEngine
{
class HorizontalLayout : public Layout
{
public:
    HorizontalLayout(const vec2ui& windowSize, Input::InputManager& inputManager);
    virtual LayoutElementWrapper& AddChild(std::unique_ptr<GuiElement>) override;
    virtual void Update() override;
    virtual void SetPostion(const vec2& position) override;
    virtual void SetPostion(const vec2ui& position) override;

private:
    float CalculateXPosition(const GuiElement&);
    void OnChange();
    void UpdateVisibility();

private:
    Input::InputManager& inputManager_;
    float totalChildrenScale_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
