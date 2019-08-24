#pragma once
#include <functional>
#include "GameEngine/Renderers/GUI/Rect.h"

namespace GameEngine
{
class GuiElement;

class LayoutElementWrapper
{
public:
    LayoutElementWrapper(GuiElement& element_, std::function<void()>);
    LayoutElementWrapper(const LayoutElementWrapper&);
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

    LayoutElementWrapper operator=(const LayoutElementWrapper&);

private:
    GuiElement& element_;
    std::function<void()> onChange_;
};
}  // namespace GameEngine
