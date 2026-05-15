#pragma once
#include <Input/InputManager.h>
#include <Input/KeysSubscriptionsManager.h>

#include <functional>
#include <optional>

#include "GameEngine/Renderers/GUI/Element.h"
#include "GameEngine/Renderers/GUI/TextInput.h"
#include "Types.h"
#include "Utils/Time/TimeMeasurer.h"

namespace GameEngine
{
namespace GUI
{
class Text;
class Sprite;
class TextInput;

class EditText : public Element
{
public:
    EditText(std::unique_ptr<Text>, std::unique_ptr<Text>, Input::InputManager&);
    EditText(const EditText&);
    ~EditText() override;
    void update() override;
    void onMouseClick(const vec2&, KeyCodes::Type) override;
    std::unique_ptr<Element> clone() const override;

    void setBackground(std::unique_ptr<Sprite>);
    void setOnEnterAction(std::function<void(const std::string&)>);
    Text* getText() const;
    const std::string& getTextString() const;
    void setText(const std::string&);
    void setTextColor(const vec3&);

private:
    void accept(IElementVisitor&) const override;

private:
    Input::InputManager& inputManager_;
    Input::KeysSubscriptionsManager keysSubscriptionsManager;

    std::string textBeforeEdit_;
    std::unique_ptr<Text> text_;
    std::unique_ptr<Text> cursor_;
    std::unique_ptr<Sprite> background_;

    std::unique_ptr<TextInput> textInput_;
    std::function<void(const std::string&)> onEnterAction_;
    Utils::Time::CTimeMeasurer timer_;
};
}  // namespace GUI
}  // namespace GameEngine
