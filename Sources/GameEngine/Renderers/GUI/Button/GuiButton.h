#pragma once
#include <Input/InputManager.h>
#include <functional>
#include "GameEngine/Renderers/GUI/GuiElement.h"
#include <Utils/Time/Timer.h>

namespace GameEngine
{
class GuiTextElement;
class GuiTextureElement;

typedef std::function<void(GuiElement&)> OnClick;

class GuiButtonElement : public GuiElement
{
public:
    GuiButtonElement(std::function<bool(GuiElement&)>, Input::InputManager&, OnClick, const vec2ui&);
    ~GuiButtonElement();
    virtual void Update() override;

    void SetText(std::unique_ptr<GuiTextElement>);
    void SetBackgroundTexture(std::unique_ptr<GuiTextureElement>);
    void SetOnHoverTexture(std::unique_ptr<GuiTextureElement>);
    void SetOnActiveTexture(std::unique_ptr<GuiTextureElement>);
    void ResetBackgroundTexture();
    void ResetOnHoverTexture();
    void ResetOnActiveTexture();


    void SetHoverTextColor(const vec3& color);
    void SetActiveTextColor(const vec3& color);

    void SetRect(const Rect& rect) override;
    void SetSize(const vec2ui& size) override;
    void SetScale(const vec2& scale) override;
    void SetPostion(const vec2& position) override;
    void SetPostion(const vec2ui& position) override;
    void SetZPosition(float z) override;
    void SetZPositionOffset(float offset) override;
    void Rotate(float r) override;
    void Show(bool) override;
    void Show() override;
    void Hide() override;
    void execute(std::function<void(uint32)>) override;
    void SetPermamanet(bool) override;
    GuiTextElement* GetText() const;
    GuiTextureElement* GetBackgroundTexture() const;
    GuiTextureElement* GetOnHoverTexture() const;
    GuiTextureElement* GetOnActiveTexture() const;
    void SetActionName(const std::string&);
    const std::string& GetActionName() const;

private:
    void SetTexture(std::unique_ptr<GuiTextureElement>&, std::unique_ptr<GuiTextureElement>&);
    void SubscribeInputAction();
    void UnsubscribeInputAction();

private:
    Input::InputManager& inputManager_;
    OnClick onClick_;

    std::unique_ptr<GuiTextElement> text_;
    std::unique_ptr<GuiTextureElement> backgroundTexture_;
    std::unique_ptr<GuiTextureElement> onHoverTexture_;
    std::unique_ptr<GuiTextureElement> onActiveTextue_;

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
