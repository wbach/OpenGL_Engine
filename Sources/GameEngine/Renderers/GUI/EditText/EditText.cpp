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
    , text_(text.get())
    , cursor_(cursor.get())
    , background_{nullptr}
    , timer_(false, 500)
{
    addChild(std::move(text));
    addChild(std::move(cursor));

    cursor_->activate(false);

    timer_.AddOnTickCallback(
        [this]()
        {
            if (textInput_)
            {
                cursor_->activate(not cursor_->isActive());
            }
        });
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
    background_ = texture.get();
    background_->setScreenScale(transform.scale);
    addChild(std::move(texture));
}

void EditText::setOnEnterAction(std::function<void(const std::string &)> f)
{
    onEnterAction_ = f;
}

Text *EditText::getText() const
{
    return text_;
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
}  // namespace GUI
}  // namespace GameEngine
