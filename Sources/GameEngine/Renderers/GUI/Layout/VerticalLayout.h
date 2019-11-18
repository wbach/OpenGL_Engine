#pragma once
#include "Layout.h"

namespace GameEngine
{
class VerticalLayout : public Layout
{
public:
    VerticalLayout(const vec2ui& windowSize, Input::InputManager& inputManager);
    ~VerticalLayout() override;
    virtual void SetScale(const vec2& scale);
    virtual void ResetView() override;
    virtual void Activate();
    virtual void Deactivate();
    float GetXOffset() const;
    void SetXOffset(float value);
    void EnableFixedSize();
    void DisableFixedSize();

private:
    void OnChange() override;
    float CalculateXPosition(const GuiElement&);
    void UpdateVisibility();
    void EnableScroll();
    void DisableScroll();
    std::vector<GuiElement*> GetAllShowed() const;
    void AdjustSize(const std::vector<GuiElement*>&);
    bool IsVisible(const GuiElement&) const;

private:
    Input::InputManager& inputManager_;
    float viewPosition_;
    std::optional<uint32> mouseWheelUpSub_;
    std::optional<uint32> mouseWheelDownSub_;
    float scrollSensitive_;
    bool adjustSize_;
    float xOffset_;

    std::vector<uint32> hiddenELements_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
