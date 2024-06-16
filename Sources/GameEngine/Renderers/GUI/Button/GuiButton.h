#pragma once
#include <Input/InputManager.h>
#include <Utils/Time/Timer.h>
#include <functional>
#include "GameEngine/Renderers/GUI/GuiElement.h"

namespace GameEngine
{
class GuiTextElement;
class GuiTextureElement;

typedef std::function<void(GuiElement&)> OnClick;

class GuiButtonElement : public GuiElement
{
    enum class State
    {
        Normal,
        Hover,
        Active
    };

public:
    GuiButtonElement(std::function<bool(const GuiElement&)>, Input::InputManager&, OnClick);
    GuiButtonElement(const GuiButtonElement&) = delete;
    ~GuiButtonElement();
    void Update() override;
    void Show() override;
    void Show(bool) override;
    GuiElement* GetCollisonElement(const vec2& mousePosition) override;

    void ExecuteAction();
    void Highlight();
    void ToneDown();
    void SetText(std::unique_ptr<GuiTextElement>);
    void SetBackgroundTexture(std::unique_ptr<GuiTextureElement>);
    void SetOnHoverTexture(std::unique_ptr<GuiTextureElement>);
    void SetOnActiveTexture(std::unique_ptr<GuiTextureElement>);
    void ResetBackgroundTexture();
    void ResetOnHoverTexture();
    void ResetOnActiveTexture();

    void SetHoverTextColor(const vec4& color);
    void SetActiveTextColor(const vec4& color);

    GuiTextElement* GetText() const;
    GuiTextureElement* GetBackgroundTexture() const;
    GuiTextureElement* GetOnHoverTexture() const;
    GuiTextureElement* GetOnActiveTexture() const;
    void SetActionName(const std::string&);
    const std::string& GetActionName() const;

private:
    void SetTexture(std::unique_ptr<GuiTextureElement>&, GuiTextureElement*&);
    void SubscribeInputAction();
    void UnsubscribeInputAction();
    State GetCurrentState() const;
    void ApplyState(State);
    void ApplyNormalState();
    void ApplyHoverState();
    void ApplyActiveState();

private:
    Input::InputManager& inputManager_;
    OnClick onClick_;

    GuiTextElement* text_;
    GuiTextureElement* backgroundTexture_;
    GuiTextureElement* onHoverTexture_;
    GuiTextureElement* onActiveTextue_;

    vec4 backgroundTextColor_;
    vec4 onHoverTextColor_;
    vec4 onActiveTextColor_;

    Utils::Timer activeTimer_;
    std::optional<uint32> subscribtion_;
    std::function<bool(const GuiElement&)> isOnTop_;
    std::string actionName_;
    State currentState_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
