#include "GuiElement.h"

#include <algorithm>

#include "Logger/Log.h"

namespace GameEngine
{
uint32 GuiElement::ID = 0;

GuiElement::GuiElement(GuiElementTypes type)
    : type_(type)
    , changeNotif_{true}
    , parent_{nullptr}
    , show_{true}
    , isActive_{true}
    , isInternal_{false}
    , id_{GuiElement::ID++}
{
}
void GuiElement::AddChild(std::unique_ptr<GuiElement> child)
{
    child->setParent(this);
    for (auto& sub : changeSubscribers_)
    {
        child->SubscribeForChange(sub);
    }

    children_.push_back(std::move(child));
    CallOnChange();
}
const std::vector<std::unique_ptr<GuiElement>>& GuiElement::GetChildren() const
{
    return children_;
}
std::vector<GuiElement*> GuiElement::GetAllShowed() const
{
    std::vector<GuiElement*> result;

    for (const auto& child : children_)
    {
        if (child->IsShow())
            result.push_back(child.get());
    }
    return result;
}
bool GuiElement::RemoveChild(uint32 id)
{
    auto iter =
        std::remove_if(children_.begin(), children_.end(), [id](const auto& child) { return child->GetId() == id; });

    if (iter != children_.end())
    {
        children_.erase(iter);
        return true;
    }
    return false;
}
void GuiElement::RemoveAll()
{
    children_.clear();
}
void GuiElement::Update()
{
    for (auto& child : children_)
    {
        child->Update();
    }
}
bool GuiElement::IsCollision(const vec2& mousePosition) const
{
    auto convertedMousePosition = (mousePosition + 1.f) / 2.f;
    auto screenPosition         = GetScreenPosition();
    auto halfScreenScale        = GetScreenScale() / 2.f;

    return convertedMousePosition.x >= screenPosition.x - halfScreenScale.x and
           convertedMousePosition.x <= screenPosition.x + halfScreenScale.x and
           convertedMousePosition.y >= screenPosition.y - halfScreenScale.y and
           convertedMousePosition.y <= screenPosition.y + halfScreenScale.y;
}
std::optional<vec2> GuiElement::GetCollisionPoint(const vec2& pos) const
{
    if (not IsCollision(pos))
    {
        return {};
    }
    return pos - transform_.position;
}
void GuiElement::SetLocalScale(const vec2& scale)
{
    transform_.scale = scale;
    CallOnChange();
}
void GuiElement::SetLocalPostion(const vec2& position)
{
    transform_.position = position;
    CallOnChange();
}
const std::string& GuiElement::GetLabel() const
{
    return label_;
}
void GuiElement::SetLabel(const std::string& label)
{
    label_ = label;
}
void GuiElement::SetStartupFunctionName(const std::string& functionName)
{
    startupFunctionName_ = functionName;
}
const std::string& GuiElement::GetStartupFunctionName() const
{
    return startupFunctionName_;
}
void GuiElement::EnableChangeNotif()
{
    changeNotif_ = true;

    for (auto& child : children_)
        child->EnableChangeNotif();
}
void GuiElement::DisableChangeNotif()
{
    changeNotif_ = false;

    for (auto& child : children_)
        child->DisableChangeNotif();
}
const GuiElementTransform& GuiElement::getTransform() const
{
    return transform_;
}
void GuiElement::SetScreenScale(const vec2& scale)
{
    if (parent_)
    {
        GuiElement::SetLocalScale(scale / parent_->GetScreenScale());
    }
    else
    {
        GuiElement::SetLocalScale(scale);
    }
}
void GuiElement::SetScreenPostion(const vec2& position)
{
    if (parent_)
    {
        SetLocalPostion(((position - parent_->GetScreenPosition()) / parent_->GetScreenScale()) + vec2(0.5f));
    }
    else
    {
        SetLocalPostion(position);
    }
}
void GuiElement::Show(bool b)
{
    show_ = b;
    CallOnChange();
}
void GuiElement::Show()
{
    show_ = true;
    CallOnChange();
}
void GuiElement::ShowPartial(uint32 depth)
{
    if (depth > 0)
    {
        for (auto& child : children_)
        {
            child->ShowPartial(depth - 1);
        }
    }
    show_ = true;
    CallOnChange();
}
void GuiElement::Hide()
{
    show_ = false;
    CallOnChange();
}

void GuiElement::Activate()
{
    isActive_ = true;
}

void GuiElement::Deactivate()
{
    isActive_ = false;
}

bool GuiElement::IsActive() const
{
    if (parent_ and not parent_->IsActive())
    {
        return false;
    }

    return isActive_;
}
const vec2& GuiElement::GetLocalScale() const
{
    return transform_.scale;
}
const vec2& GuiElement::GetLocalPosition() const
{
    return transform_.position;
}
vec2 GuiElement::GetScreenScale() const
{
    if (parent_)
    {
        return transform_.scale * parent_->GetScreenScale();
    }

    return transform_.scale;
}
vec2 GuiElement::GetScreenPosition() const
{
    if (parent_)
    {
        return parent_->GetScreenScale() * (transform_.position - vec2(0.5f)) + parent_->GetScreenPosition();
    }

    return transform_.position;
}
void GuiElement::SetZPosition(float z)
{
    transform_.zValue = z;
}
float GuiElement::GetZValue() const
{
    if (parent_)
    {
        return transform_.zValue + parent_->GetZValue();
    }
    return transform_.zValue;
}
void GuiElement::SetIsInternal()
{
    isInternal_ = true;
}
uint32 GuiElement::GetId() const
{
    return id_;
}
GuiElementTypes GuiElement::GetType() const
{
    return type_;
}
bool GuiElement::IsShow() const
{
    if (parent_ and not parent_->IsShow())
    {
        return false;
    }
    return show_;
}
void GuiElement::SetIsInternal(bool is)
{
    isInternal_ = is;

    for (auto& child : children_)
    {
        child->SetIsInternal(is);
    }
}
bool GuiElement::IsInternal() const
{
    return isInternal_;
}
bool GuiElement::CompareZValue(const GuiElement& element) const
{
    for (auto& child : children_)
    {
        if (not child->CompareZValue(element))
        {
            return false;
        }
    }
    return element.GetZValue() > GetZValue();
}
GuiElement* GuiElement::GetCollisonElement(const vec2& mousePosition)
{
    float tmin = std::numeric_limits<float>::max();
    GuiElement* result{nullptr};

    for (auto& child : children_)
    {
        if (not child->IsShow() or child->IsInternal())
        {
            continue;
        }

        auto toReturn = child->GetCollisonElement(mousePosition);

        if (toReturn)
        {
            if (toReturn->GetZValue() < tmin)
            {
                tmin   = toReturn->GetZValue();
                result = toReturn;
            }
        }
    }

    return result ? result : (IsCollision(mousePosition) ? this : nullptr);
}
GuiElement* GuiElement::Get(const std::string& label)
{
    for (auto& child : children_)
    {
        auto toReturn = child->Get(label);

        if (toReturn)
        {
            return toReturn;
        }
    }
    return label == label_ ? this : nullptr;
}

GuiElement* GuiElement::GetChild(uint32 childId)
{
    auto child = std::find_if(children_.begin(), children_.end(),
                              [childId](const auto& element) { return element->GetId() == childId; });

    return child != children_.end() ? child->get() : nullptr;
}

void GuiElement::setParent(GuiElement* parent)
{
    parent_ = parent;
}

void GuiElement::CallOnChange()
{
    if (changeNotif_)
    {
        for (auto& subscriber : changeSubscribers_)
        {
            subscriber();
        }
    }
}
void GuiElement::SubscribeForChange(std::function<void()> function)
{
    changeSubscribers_.push_back(function);
}
}  // namespace GameEngine
