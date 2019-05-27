#pragma once
#include <Input/InputManager.h>
#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "Input/KeyCodeToCharConverter.h"

namespace GameEngine
{
class GuiTextElement;

class GuiEditBoxElement : public GuiElement
{
public:
    GuiEditBoxElement(GuiTextElement&, Input::InputManager&, const vec2ui&);
    virtual void Update() override;

    void SetRect(const Rect& rect) override;
    void SetSize(const vec2ui& size) override;
    void SetScale(const vec2& scale) override;
    void SetPostion(const vec2& position) override;
    void SetPostion(const vec2ui& position) override;
    void Rotate(float r) override;
    void Show(bool) override;
    void Show() override;
    void Hide() override;

private:
    Input::InputManager& inputManager_;
    GuiTextElement& text_;
    bool inputMode_;
    Input::SingleCharType charType_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
