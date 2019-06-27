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
    virtual bool IsCollision(const vec2ui&) const;
    virtual bool IsCollision(const vec2&) const;
    virtual std::optional<vec2> GetCollisionPoint(const vec2&) const;
    virtual void SetRect(const Rect& rect);
    virtual void SetSize(const vec2ui& size);
    virtual void SetScale(const vec2& scale);
    virtual void SetPostion(const vec2& position);
    virtual void SetPostion(const vec2ui& position);
    virtual void Rotate(float r);
    virtual void Show(bool);
    virtual void Show();
    virtual void Hide();
    virtual void SetColor(const vec3& color);
    virtual const Rect& GetRect() const;
    virtual const vec2& GetScale() const;
    virtual const vec2& GetPosition() const;
    void SetZPosition(float z);

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
    float zPosition_;
    vec2 scale_;
    vec3 color_;
    mat4 transformMatrix_;
    float rotation_;
    bool show_;
};
}  // namespace GameEngine
