#include "Window.h"

#include <Logger/Log.h>

#include <magic_enum/magic_enum.hpp>
#include <memory>

#include "GameEngine/Renderers/GUI/Element.h"
#include "GameEngine/Renderers/GUI/Sprite/Sprite.h"
#include "GameEngine/Renderers/GUI/Transform.h"
#include "Input/KeyCodes.h"

namespace GameEngine
{
namespace GUI
{
Window::Window(WindowStyle style)
    : style_(style)
{
}
Window::Window(const Window& other)
    : Element(other)
    , style_(other.style_)
    , mousePosition(std::nullopt)
    , mouseReferencePosition(std::nullopt)
{
    if (other.background_)
    {
        setBackground(std::unique_ptr<Sprite>(static_cast<Sprite*>(other.background_->clone().release())));
        background_->setParent(this);
    }

    if (other.bar_)
    {
        setBar(other.bar_->clone());
    }
}

void Window::update()
{
    Element::update();

    if (not mousePosition or not mouseReferencePosition)
        return;

    auto newPosition = *mousePosition - *mouseReferencePosition;
    auto moveVec     = newPosition - transform.position;

    if (glm::length(moveVec) > std::numeric_limits<float>::epsilon())
    {
        Element::setScreenPostion(newPosition);
    }
}
WindowStyle Window::getStyle() const
{
    return style_;
}
void Window::setBackground(std::unique_ptr<Sprite> background)
{
    background->setParent(this);
    background->setZPosition(0.5f);
    background_ = std::move(background);
}

void Window::setBar(std::unique_ptr<Element> bar)
{
    LOG_DEBUG << ("Not implemented");
    // bar_ = std::move(bar);
    // bar_->setParent(this);
}
void Window::setStyle(WindowStyle s)
{
    LOG_DEBUG << "Change window style: " << magic_enum::enum_name(s);
    style_ = s;
}
void Window::onMouseClick(const vec2& position, KeyCodes::Type key)
{
    if (key == KeyCodes::LMOUSE)
    {
        mouseReferencePosition = position;
    }
}
void Window::onMouseClickRelease(KeyCodes::Type key)
{
    if (key == KeyCodes::LMOUSE)
    {
        mouseReferencePosition.reset();
    }
}
void Window::onMousePositionUpdate(const vec2& position)
{
    if (mouseReferencePosition)
    {
        mousePosition = position;
    }
}
void Window::setBackground(const Color& color)
{
    if (background_)
    {
        background_->setTextureColor(color);
    }
}
void Window::accept(IElementVisitor& visitor) const
{
    visitor.visit(*this);
}
std::unique_ptr<Element> Window::clone() const
{
    return std::make_unique<Window>(*this);
}
}  // namespace GUI
}  // namespace GameEngine
