#include "GuiEditText.h"
#include <GameEngine/Renderers/GUI/Text/GuiTextElement.h>
#include "GameEngine/Renderers/GUI/TextInput.h"
#include "GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h"

namespace GameEngine
{
GuiElementTypes GuiEditBoxElement::type = GuiElementTypes::EditBox;

GuiEditBoxElement::GuiEditBoxElement(std::unique_ptr<GuiTextElement> text, std::unique_ptr<GuiTextElement> cursor,
                                     Input::InputManager &inputManager)
    : GuiElement(type)
    , inputManager_(inputManager)
    , text_(text.get())
    , cursor_(cursor.get())
    , backgroundTexture_{nullptr}
    , timer_(false, 500)
{
    AddChild(std::move(text));
    AddChild(std::move(cursor));

    cursor_->Hide();
    lmouseSubscribtrion_ = inputManager_.SubscribeOnKeyDown(KeyCodes::LMOUSE, [this]() {
        auto position = inputManager_.GetMousePosition();

        if (IsCollision(position))
        {
            if (textInput_)
            {
                textInput_.reset();
                cursor_->Hide();
            }
            else
            {
                cursor_->Show();
                textInput_ = std::make_unique<TextInput>(inputManager_, *text_);
            }
        }
        else
        {
            if (textInput_)
            {
                textInput_.reset();
                cursor_->Hide();
            }
        }
    });

    entersubscribion_ = inputManager_.SubscribeOnKeyDown(KeyCodes::ENTER, [this]() {
        if (textInput_)
        {
            cursor_->Hide();
            textInput_.reset();
        }

        if (onEnterAction_)
        {
            onEnterAction_(text_->GetText());
        }
    });

    timer_.AddOnTickCallback([this]() {
        if (IsShow() and textInput_)
        {
            if (cursor_->IsShow())
                cursor_->Hide();
            else
                cursor_->Show();
        }
    });
}
GuiEditBoxElement::~GuiEditBoxElement()
{
    inputManager_.UnsubscribeOnKeyDown(KeyCodes::LMOUSE, lmouseSubscribtrion_);
    inputManager_.UnsubscribeOnKeyDown(KeyCodes::ENTER, entersubscribion_);
}
void GuiEditBoxElement::Update()
{
    if (not IsShow() or not textInput_)
    {
        cursor_->Hide();
        return;
    }

    timer_.StartFrame();

    if (cursor_->IsShow())
    {
        if (not text_->GetText().empty())
        {
            cursor_->SetPostion(text_->GetPosition() + vec2(text_->GetScale().x + cursor_->GetScale().x, 0));
        }
        else
        {
            cursor_->SetPostion(GetPosition());
        }
    }

    timer_.EndFrame();
}
void GuiEditBoxElement::SetBackgroundTexture(std::unique_ptr<GuiTextureElement> texture)
{
    backgroundTexture_ = texture.get();
    backgroundTexture_->SetScale(scale_);
    AddChild(std::move(texture));
}

void GuiEditBoxElement::SetOnEnterAction(std::function<void(const std::string &)> f)
{
    onEnterAction_ = f;
}

GuiTextElement *GuiEditBoxElement::GetText() const
{
    return text_;
}

const std::string &GuiEditBoxElement::GetTextString() const
{
    return text_->GetText();
}

void GuiEditBoxElement::SetText(const std::string &text)
{
    text_->SetText(text);
}

void GuiEditBoxElement::SetTextColor(const vec3 &color)
{
    text_->SetColor(color);
    cursor_->SetColor(color);
}
}  // namespace GameEngine
