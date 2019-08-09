#pragma once
#include <Input/InputManager.h>
#include <functional>
#include "GameEngine/Renderers/GUI/GuiElement.h"
#include <Utils/Time/Timer.h>

namespace GameEngine
{
class GuiTextElement;
class GuiTextureElement;

typedef std::function<void()> OnClick;

class GuiButtonElement : public GuiElement
{
public:
    GuiButtonElement(Input::InputManager&, OnClick, const vec2ui&);
    ~GuiButtonElement();
    virtual void Update() override;
    void SetText(GuiTextElement*);
    void SetBackgroundTexture(GuiTextureElement*);
    void SetOnHoverTexture(GuiTextureElement*);
    void SetOnActiveTexture(GuiTextureElement*);

    void SetHoverTextColor(const vec3& color);
    void SetActiveTextColor(const vec3& color);

    void SetRect(const Rect& rect) override;
    void SetSize(const vec2ui& size) override;
    void SetScale(const vec2& scale) override;
    void SetPostion(const vec2& position) override;
    void SetPostion(const vec2ui& position) override;
    void SetZPosition(float z) override;
    void Rotate(float r) override;
    void Show(bool) override;
    void Show() override;
    void Hide() override;
    void execute(std::function<void(uint32)>) override;

private:
    void SubscribeInputAction();
    void UnsubscribeInputAction();

private:
    Input::InputManager& inputManager_;
    OnClick onClick_;
    GuiTextElement* text_;
    GuiTextureElement* backgroundTexture_;
    GuiTextureElement* onHoverTexture_;
    GuiTextureElement* onActiveTextue_;

    vec3 backgroundTextColor_;
    vec3 onHoverTextColor_;
    vec3 onActiveTextColor_;

    Utils::Timer activeTimer_;
    size_t subscribtion_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
