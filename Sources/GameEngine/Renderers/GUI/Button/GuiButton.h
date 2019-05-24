#pragma once
#include <Input/InputManager.h>
#include <functional>
#include "GameEngine/Renderers/GUI/GuiElement.h"

namespace GameEngine
{
class GuiTextElement;
class GuiTextureElement;

typedef std::function<void()> OnClick;

class GuiButtonElement : public GuiElement
{
public:
    GuiButtonElement(Input::InputManager&, OnClick, const vec2ui&);
    virtual void Update() override;
    void SetText(GuiTextElement*);
    void SetBackgroundTexture(GuiTextureElement*);
    void SetOnHoverTexture(GuiTextureElement*);
    void SetOnActiveTexture(GuiTextureElement*);

private:
    Input::InputManager& inputManager_;
    OnClick onClick_;
    GuiTextElement* text_;
    GuiTextureElement* backgroundTexture_;
    GuiTextureElement* onHoverTexture_;
    GuiTextureElement* onActiveTextue_;

    vec3 onHoverTextColor;
    vec3 onActiveTextColor;

    vec3 onHoverTextureColor;
    vec3 onActiveTextureColor;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
