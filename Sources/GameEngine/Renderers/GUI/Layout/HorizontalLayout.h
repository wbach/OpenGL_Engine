#pragma once
#include "Layout.h"

namespace GameEngine
{
class HorizontalLayout : public Layout
{
public:
    HorizontalLayout(Input::InputManager&);
    void AddChild(std::unique_ptr<GuiElement>) override;
    void autoResize();

private:
    float calculateFirstChildXPosition() const;
    void OnChange() override;
    void calculateTotalChildrenScaleX();

private:
    Input::InputManager& inputManager_;
    float totalChildrenScaleX_;
    bool resizeAble_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
