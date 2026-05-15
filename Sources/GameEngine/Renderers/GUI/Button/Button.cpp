#include "Button.h"

#include <Logger/Log.h>

#include <algorithm>
#include <string>

#include "GameEngine/Renderers/GUI/Sprite/Sprite.h"
#include "GameEngine/Renderers/GUI/Text/Text.h"
#include "Input/KeyCodes.h"
#include "magic_enum/magic_enum.hpp"

namespace GameEngine
{
namespace GUI
{
namespace
{
const uint64 SHOW_ACTIVE_TIME = 100;
}

Button::Button()
    : text_{nullptr}
    , backgroundSprite{nullptr}
    , onHoverSprite{nullptr}
    , onActiveSprite{nullptr}
    , currentState_{State::Normal}
{
}
Button::Button(const Button &other)
    : Element(other)
    , onClick_(other.onClick_)
    , textScale(other.textScale)
    , theme(other.theme)
    , currentState_(State::Normal)
    , stickyActiveState_(other.stickyActiveState_)
    , toggleMode_(other.toggleMode_)
{
    if (other.text_)
    {
        setText(std::unique_ptr<Text>(static_cast<Text *>(other.text_->clone().release())));
    }

    if (other.backgroundSprite)
    {
        setBackground(std::unique_ptr<Sprite>(static_cast<Sprite *>(other.backgroundSprite->clone().release())));
    }

    if (other.onHoverSprite)
    {
        setOnHover(std::unique_ptr<Sprite>(static_cast<Sprite *>(other.onHoverSprite->clone().release())));
    }

    if (other.onActiveSprite)
    {
        setOnActive(std::unique_ptr<Sprite>(static_cast<Sprite *>(other.onActiveSprite->clone().release())));
    }
}
void Button::update()
{
    if (not isActive())
    {
        return;
    }

    if (not stickyActiveState_ and currentState_ == State::Active)
    {
        if (activeTimer_.GetTimeMilliseconds() > SHOW_ACTIVE_TIME)
        {
            applyState(State::Hover);
        }
    }
}

void Button::setText(std::unique_ptr<Text> text)
{
    text_ = std::move(text);

    if (text_)
    {
        text_->setLocalScale(vec2{textScale});
        text_->setZPosition(-0.2f);
        text_->setParent(this);
    }
}

void Button::setBackground(std::unique_ptr<Sprite> sprite)
{
    backgroundSprite = std::move(sprite);

    if (backgroundSprite)
    {
        backgroundSprite->setTextureColor(theme.normal.sprite.color);
        backgroundSprite->setLocalScale(vec2{1.f});
        backgroundSprite->setZPosition(-0.1f);
        backgroundSprite->setParent(this);
    }
}

void Button::setOnHover(std::unique_ptr<Sprite> sprite)
{
    onHoverSprite = std::move(sprite);

    if (onHoverSprite)
    {
        onHoverSprite->setTextureColor(theme.hover.sprite.color);
        onHoverSprite->activate(false);
        onHoverSprite->setZPosition(-0.1f);
        onHoverSprite->setLocalScale(vec2{1.f});
        onHoverSprite->setParent(this);
    }
}

void Button::setOnActive(std::unique_ptr<Sprite> sprite)
{
    onActiveSprite = std::move(sprite);

    if (onActiveSprite)
    {
        onActiveSprite->setTextureColor(theme.active.sprite.color);
        onActiveSprite->activate(false);
        onActiveSprite->setZPosition(-0.1f);
        onActiveSprite->setLocalScale(vec2{1.f});
        onActiveSprite->setParent(this);
    }
}

void Button::setOnHover(const Color &color)
{
    theme.hover.sprite.color = color;

    if (onHoverSprite)
    {
        onHoverSprite->setTextureColor(color);
    }
}

void Button::setBackground(const Color &color)
{
    theme.normal.sprite.color = color;

    if (backgroundSprite)
    {
        backgroundSprite->setTextureColor(color);
    }
}

void Button::setOnActive(const Color &color)
{
    theme.active.sprite.color = color;

    if (onActiveSprite)
    {
        onActiveSprite->setTextureColor(color);
    }
}

void Button::setOnHoverText(const Color &color)
{
    theme.hover.text.color = color;
}

void Button::setBackgroundText(const Color &color)
{
    theme.normal.text.color = color;

    if (text_)
    {
        text_->setTextureColor(color);
    }
}

void Button::setOnActiveText(const Color &color)
{
    theme.active.text.color = color;
}

const std::string &Button::getText() const
{
    if (text_)
    {
        return text_->getText();
    }

    static std::string empty;
    return empty;
}

Text *Button::getTextElement() const
{
    return text_.get();
}

Sprite *Button::getBackgroundSprite() const
{
    return backgroundSprite.get();
}

Sprite *Button::getOnHoverSprite() const
{
    return onHoverSprite.get();
}

Sprite *Button::getOnActiveSprite() const
{
    return onActiveSprite.get();
}

void Button::applyState(State state)
{
    switch (state)
    {
        case State::Normal:
            applyNormalState();
            break;
        case State::Hover:
            applyHoverState();
            break;
        case State::Active:
            applyActiveState();
            break;
    }

    currentState_ = state;
}

void Button::applyNormalState()
{
    if (onHoverSprite)
        onHoverSprite->activate(false);
    if (onActiveSprite)
        onActiveSprite->activate(false);
    if (backgroundSprite)
        backgroundSprite->activate(true);
    if (text_)
        text_->setTextureColor(theme.normal.text.color);
}

void Button::applyHoverState()
{
    if (not onHoverSprite)
        return;

    if (backgroundSprite)
        backgroundSprite->activate(false);
    if (onActiveSprite)
        onActiveSprite->activate(false);
    if (onHoverSprite)
        onHoverSprite->activate(true);
    if (text_)
        text_->setTextureColor(theme.hover.text.color);
}

void Button::applyActiveState()
{
    if (not onActiveSprite)
        return;

    if (backgroundSprite)
        backgroundSprite->activate(false);
    if (onHoverSprite)
        onHoverSprite->activate(false);
    if (onActiveSprite)
        onActiveSprite->activate(true);
    if (text_)
        text_->setTextureColor(theme.active.text.color);
}
const Color &Button::getBackgroundColor() const
{
    return theme.normal.text.color;
}
const Color &Button::getActiveColor() const
{
    return theme.active.text.color;
}
const Color &Button::getOnHoverColor() const
{
    return theme.hover.text.color;
}
void Button::setOnClick(OnClick func)
{
    onClick_ = func;
}
void Button::onMouseClick(const vec2 &, KeyCodes::Type key)
{
    if (not isActive())
    {
        return;
    }
    if (key == KeyCodes::LMOUSE)
    {
        if (not toggleMode_ and currentState_ != State::Active)
        {
            applyState(State::Active);
            activeTimer_.Reset();
        }
        else
        {
            applyState(State::Hover);
        }

        if (onClick_)
        {
            onClick_();
        }
    }
}
void Button::onMouseEnter()
{
    if (stickyActiveState_ and currentState_ == State::Active)
        return;

    applyState(State::Hover);
}
void Button::onMouseLeave()
{
    if (stickyActiveState_ and currentState_ == State::Active)
        return;

    applyState(State::Normal);
}
void Button::accept(IElementVisitor &visitor) const
{
    visitor.visit(*this);
}
void Button::refreshSelf()
{
}
void Button::setTextScale(float scale)
{
    textScale = scale;

    if (text_)
    {
        text_->setLocalScale(vec2{scale});
    }
}
float Button::getTextScale() const
{
    return textScale;
}
void Button::highlight()
{
    if (stickyActiveState_ and currentState_ == State::Active)
        return;

    applyHoverState();
}
void Button::toneDown()
{
    if (stickyActiveState_ and currentState_ == State::Active)
        return;

    applyState(currentState_);
}
void Button::executeAction()
{
    if (onClick_)
        onClick_();
}
void Button::setTheme(const Theme::Button &t)
{
    theme = t;
}
std::unique_ptr<Element> Button::clone() const
{
    return std::make_unique<Button>(*this);
}
void Button::resetActiveState()
{
    applyState(State::Normal);
}
bool Button::isToogleMode() const
{
    return toggleMode_;
}
void Button::setToogleMode(bool v)
{
    toggleMode_ = v;
}
bool Button::isStickyActive() const
{
    return stickyActiveState_;
}
void Button::setStickyActiveState(bool v)
{
    stickyActiveState_ = v;
}
void Button::setAsActive()
{
    applyState(State::Active);
}
}  // namespace GUI
}  // namespace GameEngine
