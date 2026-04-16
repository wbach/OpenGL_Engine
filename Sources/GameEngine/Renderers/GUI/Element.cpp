#include "Element.h"

#include <Logger/Log.h>

#include <algorithm>
#include <optional>

#include "IdPool.h"

namespace GameEngine
{
namespace GUI
{
namespace
{
static Utils::IdPool idPool;
}
Element::Element()
    : parent{nullptr}
    , active{true}
    , id{idPool.getId()}
{
}
Element::Element(const Element& other)
    : margin_(other.margin_)
    , parent(nullptr)
    , transform(other.transform)
    , label(other.label)
    , mouseFocused(false)
    , isDirty(true)
    , active(other.active)
    , id{idPool.getId()}
{
}
Element::~Element()
{
    idPool.releaseId(id);
}
void Element::addChild(std::unique_ptr<Element> child)
{
    child->setParent(this);
    children.push_back(std::move(child));
    invalidate();
}
const std::vector<std::unique_ptr<Element>>& Element::getChildren() const
{
    return children;
}
bool Element::removeChild(uint32 id)
{
    auto iter = std::remove_if(children.begin(), children.end(), [id](const auto& child) { return child->getId() == id; });

    if (iter != children.end())
    {
        children.erase(iter);
        invalidate();
        return true;
    }

    for (const auto& child : children)
    {
        if (child->removeChild(id))
            return true;
    }
    return false;
}
void Element::removeAll()
{
    children.clear();
}
void Element::update()
{
    for (auto& child : children)
    {
        child->update();
    }
}
void Element::setLocalScale(const vec2& scale)
{
    transform.scale = scale;
    invalidate();
}
void Element::setLocalPosition(const vec2& position)
{
    transform.position = position;
    invalidate();
}
const std::string& Element::getLabel() const
{
    return label;
}
void Element::setLabel(const std::string& labelname)
{
    label = labelname;
}
const Transform& Element::getTransform() const
{
    return transform;
}
void Element::setScreenScale(const vec2& scale)
{
    if (parent)
    {
        setLocalScale(scale / parent->getScreenScale());
    }
    else
    {
        setLocalScale(scale);
    }
}
void Element::setScreenPostion(const vec2& position)
{
    if (parent)
    {
        setLocalPosition(((position - parent->getScreenPosition()) / parent->getScreenScale()) + vec2(0.5f));
    }
    else
    {
        setLocalPosition(position);
    }
}

void Element::activate(bool v)
{
    active = v;
    invalidate();
}

void Element::activateInternal(bool v)
{
    active = v;
}

bool Element::isActive() const
{
    if (parent)
    {
        return parent->isActive() and active;
    }
    return active;
}
const vec2& Element::getLocalScale() const
{
    return transform.scale;
}
const vec2& Element::getLocalPosition() const
{
    return transform.position;
}
vec2 Element::getScreenScale() const
{
    if (parent)
    {
        return transform.scale * parent->getScreenScale();
    }

    return transform.scale;
}
vec2 Element::getScreenPosition() const
{
    if (parent)
    {
        return parent->getScreenScale() * (transform.position - vec2(0.5f)) + parent->getScreenPosition();
    }

    return transform.position;
}
void Element::setZPosition(float z)
{
    transform.zValue = z;
    invalidate();
}
float Element::getZValue() const
{
    if (parent)
    {
        return transform.zValue + parent->getZValue() - 0.001f;
    }
    return transform.zValue;
}
uint32 Element::getId() const
{
    return id;
}
bool Element::compareZValue(const Element& element) const
{
    for (auto& child : children)
    {
        if (not child->compareZValue(element))
        {
            return false;
        }
    }
    return element.getZValue() > getZValue();
}
Element* Element::get(const Label& input)
{
    if (input == label)
        return this;

    for (auto& child : children)
    {
        if (auto toReturn = child->get(input))
        {
            return toReturn;
        }
    }
    return nullptr;
}

Element* Element::get(IdType inputId)
{
    if (id == inputId)
        return this;

    for (auto& child : children)
    {
        if (Element* found = child->get(inputId))
        {
            return found;
        }
    }

    return nullptr;
}
void Element::setParent(Element* ptr)
{
    parent = ptr;
}

void Element::setTransform(const Transform& t)
{
    transform = t;
    invalidate();
}
void Element::onMouseClick(const vec2&, KeyCodes::Type)
{
}
void Element::onMouseEnter()
{
    mouseFocused = true;
}
void Element::onMouseLeave()
{
    mouseFocused = false;
}
void Element::onMousePositionUpdate(const vec2&)
{
}
void Element::onMouseClickRelease(KeyCodes::Type)
{
}
bool Element::onMouseWheel(const vec2& delta)
{
    if (parent)
    {
        return parent->onMouseWheel(delta);
    }
    return false;
}
void Element::accept(IElementVisitor& v)
{
    v.visit(*this);
}
void Element::invalidate()
{
    isDirty = true;
    if (parent and not parent->isDirty)
    {
        parent->invalidate();
    }
}
void Element::refresh()
{
    if (not isDirty)
    {
        return;
    }

    for (auto& child : children)
    {
        child->refresh();
    }

    refreshSelf();

    isDirty = false;
}
void Element::refreshSelf()
{
}
void Element::setInternalPosition(const vec2& position)
{
    transform.position = position;
}
std::unique_ptr<Element> Element::clone() const
{
    return std::make_unique<Element>(*this);
};
Element* Element::getParent()
{
    return parent;
};
void Element::reorderChildUp(const Element& child)
{
    auto it = std::find_if(children.begin(), children.end(), [&child](const auto& ptr) { return ptr->getId() == child.getId(); });
    if (it != children.begin() and it != children.end())
    {
        std::iter_swap(it, std::prev(it));
        invalidate();
    }
}
void Element::reorderChildDown(const Element& child)
{
    auto it = std::find_if(children.begin(), children.end(), [&child](const auto& ptr) { return ptr->getId() == child.getId(); });
    if (it != children.end() and std::next(it) != children.end())
    {
        std::iter_swap(it, std::next(it));
        invalidate();
    }
}
void Element::setMargin(const Spacing& s)
{
    margin_ = s;
    invalidate();
}
const Spacing& Element::getMargin() const
{
    return margin_;
}
}  // namespace GUI
}  // namespace GameEngine
