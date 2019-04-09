#pragma once
#include "GLM/GLMUtils.h"
#include "GuiElementTypes.h"
#include "Rect.h"

namespace GameEngine
{
class GuiElement
{
public:
    GuiElement(GuiElementTypes& type, const vec2ui& windowSize);
    virtual ~GuiElement() = default;

public:
    void SetRect(const Rect& rect);
    void SetScale(const vec2& size);
    void SetPostion(const vec2& position);
    void SetPostion(const vec2ui& position);
    void Rotate(float r);
    void Show(bool);
    void Show();
    void Hide();
    void SetColor(const vec3& color);

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
    vec2ui windowSize_;
    GuiElementTypes type_;

protected:
    Rect rect_;
    vec2 position_;
    vec2 scale_;
    vec3 color_;
    mat4 transformMatrix_;
    float rotation_;
    bool show_;
};
}  // namespace GameEngine
