#pragma once
#include <Input/InputManager.h>
#include <functional>
#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "GameEngine/Renderers/GUI/TextInput.h"
#include "Utils/Time/TimeMeasurer.h"

namespace GameEngine
{
class GuiTextureElement;
class GuiTextElement;
class TextInput;

class GuiEditBoxElement : public GuiElement
{
public:
    GuiEditBoxElement(std::unique_ptr<GuiTextElement>, std::unique_ptr<GuiTextElement>, Input::InputManager&, const vec2ui&);
    ~GuiEditBoxElement();
    virtual void Update() override;

    void SetBackgroundTexture(std::unique_ptr<GuiTextureElement>);
    void SetRect(const Rect& rect) override;
    void SetSize(const vec2ui& size) override;
    void SetScale(const vec2& scale) override;
    void SetPostion(const vec2& position) override;
    void SetPostion(const vec2ui& position) override;
    void SetZPositionOffset(float offset) override;
    void SetZPosition(float z) override;
    void SetOnEnterAction(std::function<void(const std::string&)>);
    void Rotate(float r) override;
    void Show(bool) override;
    void Show() override;
    void Hide() override;
    const std::string& GetTextString() const;
    void SetText(const std::string&);
    void SetTextColor(const vec3& color);
    void SetPermamanet(bool) override;

private:
    Input::InputManager& inputManager_;

    std::unique_ptr<GuiTextElement> text_;
    std::unique_ptr<GuiTextElement> cursor_;
    std::unique_ptr<GuiTextureElement> backgroundTexture_;

    uint32 lmouseSubscribtrion_;
    uint32 entersubscribion_;
    std::unique_ptr<TextInput> textInput_;
    std::function<void(const std::string&)> onEnterAction_;
    Utils::Time::CTimeMeasurer timer_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
