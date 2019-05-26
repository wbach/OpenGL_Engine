#include "GuiElement.h"
#include "Logger/Log.h"

namespace GameEngine
{
float ConvertSpace(float v)
{
    return (v + 1.f) / 2.f;
}

float ConvertSpaceInv(float v)
{
    return v * 2.f - 1.f;
}

GuiElement::GuiElement(GuiElementTypes type, const vec2ui& windowSize)
    : type_(type)
    , windowSize_{windowSize}
    , rect_{{0, 0}, {100, 100}}
    , position_{0, 0}
    , zPosition_{0}
    , scale_{0.25, 0.25}
    , color_{1, 1, 1}
    , transformMatrix_(1.f)
    , rotation_{0}
    , show_{true}
{
    CalculateMatrix();
}
void GuiElement::Update()
{
}
bool GuiElement::IsCollision(const vec2ui& pos) const
{
    return pos.x >= rect_.position.x and pos.x <= rect_.position.x + rect_.size.x and pos.y >= rect_.position.y and pos.y <= rect_.position.y + rect_.size.y;
}

bool GuiElement::IsCollision(const vec2& pos) const
{
    auto convertedPos = pos;
    return convertedPos.x >= position_.x - scale_.x and convertedPos.x <= position_.x + scale_.x and convertedPos.y >= position_.y - scale_.y and convertedPos.y <= position_.y + scale_.y;
}
std::optional<vec2> GuiElement::GetCollisionPoint(const vec2& pos) const
{
    if (not IsCollision(pos))
    {
        return {};
    }
    return pos - position_;
}
void GuiElement::SetRect(const Rect& rect)
{
    rect_ = rect;
    CalculatePosition();
    CaclulateScaleBasedOnRect();
    CalculateMatrix();
}
void GuiElement::SetSize(const vec2ui& size)
{
    rect_.size = size;
    CaclulateScaleBasedOnRect();
    CalculateMatrix();
}
void GuiElement::SetScale(const vec2& scale)
{
    scale_ = scale;
    CalculateRectBasedOnScale();
    CalculateMatrix();
}
void GuiElement::SetPostion(const vec2& position)
{
    position_ = position;
    CalcualteRectPosition();
    CalculateMatrix();
}
void GuiElement::SetPostion(const vec2ui& position)
{
    rect_.position = position;
    CalculatePosition();
    CalculateMatrix();
}
void GuiElement::Rotate(float r)
{
    rotation_ = r;
    CalculateMatrix();
}
void GuiElement::Show(bool b)
{
    show_ = b;
}
void GuiElement::Show()
{
    show_ = true;
}
void GuiElement::Hide()
{
    show_ = false;
}
void GuiElement::SetColor(const vec3& color)
{
    color_ = color;
}
const Rect& GuiElement::GetRect() const
{
    return rect_;
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
    zPosition_ = z;
    CalculateMatrix();
}
GuiElementTypes GuiElement::GetType() const
{
    return type_;
}
bool GuiElement::IsShow() const
{
    return show_;
}
const mat4& GuiElement::GetMatrix() const
{
    return transformMatrix_;
}
const vec3& GuiElement::GetColor() const
{
    return color_;
}
void GuiElement::CalculateMatrix()
{
    transformMatrix_ = Utils::CreateTransformationMatrix(vec3(position_, zPosition_), scale_, rotation_);
}
void GuiElement::CaclulateScaleBasedOnRect()
{
    scale_ = vec2(rect_.size.x, rect_.size.y);
    scale_.x *= 1.f / (windowSize_.x * 2.f);
    scale_.y *= 1.f / (windowSize_.y * 2.f);
}
void GuiElement::CalculateRectBasedOnScale()
{
    rect_.size.x = static_cast<uint32>(scale_.x * static_cast<float>(windowSize_.x)) * 2;
    rect_.size.y = static_cast<uint32>(scale_.y * static_cast<float>(windowSize_.y)) * 2;
}
void GuiElement::CalcualteRectPosition()
{
    rect_.position.x = static_cast<uint32>(ConvertSpace(position_.x) * static_cast<float>(windowSize_.x));
    rect_.position.y = static_cast<uint32>(ConvertSpace(position_.y) * static_cast<float>(windowSize_.y));
}
void GuiElement::CalculatePosition()
{
    position_ = vec2(rect_.position.x, rect_.position.y);
    position_.x /= windowSize_.x;
    position_.y /= windowSize_.y;

    position_.x = ConvertSpaceInv(position_.x);
    position_.y = ConvertSpaceInv(position_.y);
}
}  // namespace GameEngine
