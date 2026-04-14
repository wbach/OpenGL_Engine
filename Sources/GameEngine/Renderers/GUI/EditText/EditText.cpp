#include "EditText.h"

#include <GameEngine/Renderers/GUI/Text/Text.h>

#include "GameEngine/Renderers/GUI/Sprite/Sprite.h"
#include "GameEngine/Renderers/GUI/TextInput.h"
#include "Input/KeyCodes.h"

namespace GameEngine
{
namespace GUI
{
EditText::EditText(std::unique_ptr<Text> text, std::unique_ptr<Text> cursor, Input::InputManager &inputManager)
    : inputManager_(inputManager)
    , keysSubscriptionsManager(inputManager)
    , text_(std::move(text))
    , cursor_(std::move(cursor))
    , background_{nullptr}
    , timer_(false, 500)
{
    cursor_->activate(false);

    if (text_)
        text_->setParent(this);
    if (cursor_)
        cursor_->setParent(this);

    timer_.AddOnTickCallback(
        [this]()
        {
            if (textInput_)
            {
                cursor_->activate(not cursor_->isActive());
            }
        });
}

EditText::EditText(const EditText &other)
    : Element(other)
    , inputManager_(other.inputManager_)
    , keysSubscriptionsManager(other.inputManager_)
    , textBeforeEdit_(other.textBeforeEdit_)
    , onEnterAction_(other.onEnterAction_)
{
    if (other.text_)
    {
        text_ = std::unique_ptr<Text>(static_cast<Text *>(other.text_->clone().release()));
        text_->setParent(this);
    }

    if (other.cursor_)
    {
        cursor_ = std::unique_ptr<Text>(static_cast<Text *>(other.cursor_->clone().release()));
        cursor_->setParent(this);
    }

    if (other.background_)
    {
        background_ = std::unique_ptr<Sprite>(static_cast<Sprite *>(other.background_->clone().release()));
        background_->setParent(this);
    }

    textInput_ = nullptr;

}
EditText::~EditText()
{
}
void EditText::update()
{
    if (not isActive() or not textInput_)
    {
        cursor_->activate(false);
        return;
    }

    timer_.StartFrame();

    if (not text_->getText().empty())
    {
        cursor_->setLocalPosition(text_->getLocalPosition() + vec2(text_->getLocalScale().x + cursor_->getLocalScale().x, 0));
    }
    else
    {
        cursor_->setLocalPosition(getLocalPosition());
    }

    timer_.EndFrame();
}
void EditText::setBackground(std::unique_ptr<Sprite> texture)
{
    background_ = std::move(texture);
    if (background_)
    {
        background_->setScreenScale(transform.scale);
        background_->setParent(this);
    }
}

void EditText::setOnEnterAction(std::function<void(const std::string &)> f)
{
    onEnterAction_ = f;
}

Text *EditText::getText() const
{
    return text_.get();
}

const std::string &EditText::getTextString() const
{
    return text_->getText();
}

void EditText::setText(const std::string &text)
{
    text_->setText(text);
}

void EditText::setTextColor(const vec3 &color)
{
    text_->setColor(color);
    cursor_->setColor(color);
}
void EditText::onMouseClick(const vec2 &position, KeyCodes::Type key)
{
    if (key != KeyCodes::LMOUSE)
        return;

    if (not text_)
        return;

    textBeforeEdit_ = text_->getText();

    if (textInput_)
    {
        textInput_.reset();
        cursor_->activate(false);
    }
    else
    {
        cursor_->activate(true);
        textInput_ = std::make_unique<TextInput>(inputManager_, *text_);
    }

    keysSubscriptionsManager = inputManager_.SubscribeOnKeyDown(KeyCodes::ENTER,
                                                                [this]()
                                                                {
                                                                    if (textInput_)
                                                                    {
                                                                        cursor_->activate(false);
                                                                        textInput_.reset();
                                                                    }
                                                                    if (onEnterAction_ and text_)
                                                                    {
                                                                        onEnterAction_(text_->getText());
                                                                    }

                                                                    keysSubscriptionsManager.UnsubscribeKeys();
                                                                });
    keysSubscriptionsManager = inputManager_.SubscribeOnKeyDown(KeyCodes::ESCAPE,
                                                                [this]()
                                                                {
                                                                    if (textInput_)
                                                                    {
                                                                        cursor_->activate(false);
                                                                        textInput_.reset();
                                                                    }

                                                                    if (text_)
                                                                    {
                                                                        text_->setText(textBeforeEdit_);
                                                                    }

                                                                    keysSubscriptionsManager.UnsubscribeKeys();
                                                                });
    keysSubscriptionsManager = inputManager_.SubscribeOnKeyDown(KeyCodes::LMOUSE,
                                                                [this]()
                                                                {
                                                                    if (not mouseFocused)
                                                                    {
                                                                        if (textInput_)
                                                                        {
                                                                            cursor_->activate(false);
                                                                            textInput_.reset();
                                                                        }
                                                                        if (text_)
                                                                        {
                                                                            text_->setText(textBeforeEdit_);
                                                                        }
                                                                        keysSubscriptionsManager.UnsubscribeKeys();
                                                                    }
                                                                });
}
void EditText::accept(IElementVisitor &visitor)
{
    visitor.visit(*this);
}
std::unique_ptr<Element> EditText::clone() const
{
    return std::make_unique<EditText>(*this);
}
}  // namespace GUI
}  // namespace GameEngine
