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
    void SetOnEnterAction(std::function<void(const std::string&)>);
    GuiTextElement* GetText() const;
    const std::string& GetTextString() const;
    void SetText(const std::string&);
    void SetTextColor(const vec3& color);

private:
    Input::InputManager& inputManager_;

    std::string defaultText_;
    GuiTextElement* text_;
    GuiTextElement* cursor_;
    GuiTextureElement* backgroundTexture_;

    uint32 lmouseSubscribtrion_;
    uint32 entersubscribion_;
    std::unique_ptr<TextInput> textInput_;
    std::function<void(const std::string&)> onEnterAction_;
    Utils::Time::CTimeMeasurer timer_;

public:
    static GuiElementTypes type;
};
}  // namespace GameEngine
