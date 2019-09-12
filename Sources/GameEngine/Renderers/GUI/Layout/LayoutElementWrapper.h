#pragma once
#include <functional>
#include "GameEngine/Renderers/GUI/Rect.h"
#include <memory>

namespace GameEngine
{
class GuiElement;

class LayoutElementWrapper
{
public:
    LayoutElementWrapper(std::unique_ptr<GuiElement> element_, std::function<void()>);
    LayoutElementWrapper(const LayoutElementWrapper&) = delete;
    void Update();
    void SetRect(const Rect& rect);
    void SetSize(const vec2ui& size);
    void SetScale(const vec2& scale);
    void SetPostion(const vec2& position);
    void SetPostion(const vec2ui& position);
    void SetPositionWithoutNotif(const vec2& position);
    void SetZPosition(float z);
    void SetZPositionOffset(float offset);
    void Rotate(float r);
    void Show(bool);
    void Show();
    void ShowWithoutNotif();
    void Hide();
    void HideWithoutNotif();
    void MarkToRemove();
    uint32 GetId() const;
    const GuiElement& Get() const;
    void SetPermamanet(bool);
    bool CompareZValue(const GuiElement&) const;

private:
    std::unique_ptr<GuiElement> element_;
    std::function<void()> onChange_;
};
}  // namespace GameEngine
