#pragma once
#include <unordered_map>
#include "GLM/GLMUtils.h"
#include "Types.h"

namespace GameEngine
{
class Texture;

namespace Renderer
{
namespace Gui
{
class GuiTextureElement
{
public:
    Texture* texture = nullptr;

    void SetPosition(const vec2& pos)
    {
        position_ = pos;
        UpdateTransformMatrix();
    }
    void SetRotation(float r)
    {
        rotation_ = r;
        UpdateTransformMatrix();
    }
    void SetScale(const vec2& scale)
    {
        scale_ = scale;
        UpdateTransformMatrix();
    }
    const mat4& GetMatrix() const
    {
        return transformMatrix_;
    }
    const vec2 GetPosition() const
    {
        return position_;
    }
    const vec2 GetScale() const
    {
        return scale_;
    }
    float GetRotation()
    {
        return rotation_;
    }

private:
    void UpdateTransformMatrix()
    {
        transformMatrix_ = Utils::CreateTransformationMatrix(position_, scale_, rotation_);
    }

private:
    mat4 transformMatrix_ = mat4(1.f);
    vec2 position_        = vec2(0);
    vec2 scale_           = vec2(1);
    float rotation_       = 0.f;
};
}  // Gui
}  // Renderers
}  // GameEngine
