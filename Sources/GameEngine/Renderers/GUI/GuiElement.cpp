#include "GuiElement.h"
#include <algorithm>
#include "Logger/Log.h"

namespace GameEngine
{
uint32 GuiElement::ID = 0;

GuiElement::GuiElement(GuiElementTypes type, const vec2ui& windowSize)
    : type_(type)
    , changeNotif_{true}
    , windowSize_{windowSize}
    , position_{0, 0}
    , scale_{0.05, 0.05}
    , show_{true}
    , isInternal_{false}
    , isMarkToRemove_{false}
    , id_{GuiElement::ID++}
{
}
void GuiElement::AddChild(std::unique_ptr<GuiElement> child)
{
    child->SetZPositionOffset(GetZValue());
    child->Show(IsShow());
    children_.push_back(std::move(child));
    CallOnChange();
}
const std::vector<std::unique_ptr<GuiElement>>& GuiElement::GetChildren() const
{
    return children_;
}
void GuiElement::RemoveChild(uint32 id)
{
    auto iter =
        std::remove_if(children_.begin(), children_.end(), [id](const auto& child) { return child->GetId() == id; });

    children_.erase(iter);
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
    return mousePosition.x >= position_.x - scale_.x and mousePosition.x <= position_.x + scale_.x and
           mousePosition.y >= position_.y - scale_.y and mousePosition.y <= position_.y + scale_.y;
}
std::optional<vec2> GuiElement::GetCollisionPoint(const vec2& pos) const
{
    if (not IsCollision(pos))
    {
        return {};
    }
    return pos - position_;
}
void GuiElement::SetScale(const vec2& scale)
{
    scale_ = scale;
    for (auto& child : children_)
    {
        if (not child->isInternal_)
        child->SetScale(scale);
    }
    CallOnChange();
}
void GuiElement::SetPostion(const vec2& position)
{
    auto moveVec = position - position_;
    for (auto& child : children_)
    {
        UpdatePosition(*child, moveVec);
    }
    position_ = position;
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
}
void GuiElement::DisableChangeNotif()
{
    changeNotif_ = false;
}
void GuiElement::Show(bool b)
{
    for (auto& child : children_)
    {
        child->Show(b);
    }

    show_ = b;
    CallOnChange();
}
void GuiElement::Show()
{
    for (auto& child : children_)
    {
        child->Show();
    }

    show_ = true;
    CallOnChange();
}
void GuiElement::Hide()
{
    for (auto& child : children_)
    {
        child->Hide();
    }
    show_ = false;
    CallOnChange();
}
const vec2& GuiElement::GetScale() const
{
    return scale_;
}
const vec2& GuiElement::GetPosition() const
{
    return position_;
}
void GuiElement::SetZPosition(float z)
{
    zPosition_.value  = z;
    zPosition_.total_ = zPosition_.value + zPosition_.offset_;

    for (auto& child : children_)
    {
        child->SetZPositionOffset(GetZValue());
    }
}
void GuiElement::SetZPositionOffset(float offset)
{
    zPosition_.offset_ = offset;
    zPosition_.total_  = zPosition_.value + zPosition_.offset_;

    for (auto& child : children_)
    {
        child->SetZPositionOffset(GetZValue());
    }
}
float GuiElement::GetZValue() const
{
    return zPosition_.total_;
}

void GuiElement::SetIsInternal()
{
    isInternal_ = true;
}
uint32 GuiElement::GetId() const
{
    return id_;
}
void GuiElement::MarkToRemove()
{
    isMarkToRemove_ = true;
}
bool GuiElement::IsMarkToRemove() const
{
    return isMarkToRemove_;
}
GuiElementTypes GuiElement::GetType() const
{
    return type_;
}
bool GuiElement::IsShow() const
{
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
                tmin = toReturn->GetZValue();
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

void GuiElement::CallOnChange()
{
    if (onChange_ and changeNotif_)
    {
        onChange_();

        for (auto& child : children_)
        {
            child->CallOnChange();
        }
    }
}

void GuiElement::UpdatePosition(GuiElement& element, const vec2& v)
{
    auto position = element.GetPosition();
    position      = position + v;
    element.SetPostion(position);
}
void GuiElement::SetOnchangeFunction(std::function<void()> function)
{
    for (auto& child : children_)
    {
        child->SetOnchangeFunction(function);
    }

    onChange_ = function;
}
}  // namespace GameEngine
