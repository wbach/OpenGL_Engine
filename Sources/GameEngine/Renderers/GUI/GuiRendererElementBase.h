#pragma once
#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "GameEngine/Resources/Textures/Texture.h"

namespace GameEngine
{
class GuiRendererElementBase : public GuiElement
{
public:
    GuiRendererElementBase(std::function<void(GuiElement&)> renderSubscribe,
                           std::function<void(const GuiElement&)> unsubscribeElement, GuiElementTypes type,
                           const vec2ui& windowSize);
    ~GuiRendererElementBase() override;

    void SetScale(const vec2& scale) override;
    void SetPostion(const vec2& position) override;

    void SetColor(const vec3& color);
    inline std::optional<uint32> GetTextureId() const;
    inline const mat4& GetTransformMatrix() const;
    inline const vec3& GetColor() const;

protected:
    void CalculateMatrix();

protected:
    Texture* texture_;
    vec3 color_;
    vec2 offset_;
    mat4 transformMatrix_;
    std::function<void(const GuiElement&)> unsubscribeElement_;
};

std::optional<uint32> GuiRendererElementBase::GetTextureId() const
{
    return texture_ ? texture_->GetId() : std::optional<uint32>();
}
const mat4& GuiRendererElementBase::GetTransformMatrix() const
{
    return transformMatrix_;
}
const vec3& GuiRendererElementBase::GetColor() const
{
    return color_;
}
}  // namespace GameEngine
