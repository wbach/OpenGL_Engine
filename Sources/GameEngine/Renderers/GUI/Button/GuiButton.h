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
public:
    GuiButtonElement(std::function<bool(GuiElement&)>, Input::InputManager&, OnClick, const vec2ui&);
    GuiButtonElement(const GuiButtonElement&) = delete;
    ~GuiButtonElement();
    void Update() override;
    GuiElement* GetCollisonElement(const vec2& mousePosition) override;

    void SetText(std::unique_ptr<GuiTextElement>);
    void SetBackgroundTexture(std::unique_ptr<GuiTextureElement>);
    void SetOnHoverTexture(std::unique_ptr<GuiTextureElement>);
    void SetOnActiveTexture(std::unique_ptr<GuiTextureElement>);
    void ResetBackgroundTexture();
    void ResetOnHoverTexture();
    void ResetOnActiveTexture();

    void SetHoverTextColor(const vec3& color);
    void SetActiveTextColor(const vec3& color);

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
    std::optional<uint32> subscribtion_;
    std::function<bool(GuiElement&)> isOnTop_;
    std::string actionName_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
