#pragma once
#include "GLM/GLMUtils.h"
#include "GuiElementTypes.h"
#include "Rect.h"
#include <optional>

namespace GameEngine
{
class GuiElement
{
public:
    GuiElement(GuiElementTypes type, const vec2ui& windowSize);
    virtual ~GuiElement() = default;

public:
    virtual void Update();
    bool IsCollision(const vec2ui&) const;
    bool IsCollision(const vec2&) const;
    std::optional<vec2> GetCollisionPoint(const vec2&) const;
    void SetRect(const Rect& rect);
    void SetSize(const vec2ui& size);
    void SetScale(const vec2& scale);
    void SetPostion(const vec2& position);
    void SetPostion(const vec2ui& position);
    void Rotate(float r);
    void Show(bool);
    void Show();
    void Hide();
    void SetColor(const vec3& color);
    const Rect& GetRect() const;
    const vec2& GetScale() const;
    const vec2& GetPosition() const;

public:
    GuiElementTypes GetType() const;
    bool IsShow() const;
    const mat4& GetMatrix() const;
    const vec3& GetColor() const;

protected:
    void CalculateMatrix();

private:
    void CaclulateScaleBasedOnRect();
    void CalculateRectBasedOnScale();
    void CalcualteRectPosition();
    void CalculatePosition();

private:
    GuiElementTypes type_;

protected:
    vec2ui windowSize_;
    Rect rect_;
    vec2 position_;
    vec2 scale_;
    vec3 color_;
    mat4 transformMatrix_;
    float rotation_;
    bool show_;
};
}  // namespace GameEngine
