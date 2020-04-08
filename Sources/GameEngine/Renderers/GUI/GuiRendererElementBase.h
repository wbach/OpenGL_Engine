#pragma once
#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "GameEngine/Resources/Textures/Texture.h"

namespace GameEngine
{
class GUIRenderer;

class GuiRendererElementBase : public GuiElement
{
public:
    GuiRendererElementBase(GUIRenderer& guiRenderer, GuiElementTypes type,
                           const vec2ui& windowSize);
    ~GuiRendererElementBase() override;

    void SetScale(const vec2& scale) override;
    void SetPostion(const vec2& position) override;

    void SetColor(const vec3& color);
    void SetColor(const vec4& color);
    inline std::optional<uint32> GetTextureId() const;
    inline const mat4& GetTransformMatrix() const;
    inline const vec4& GetColor() const;

protected:
    void CalculateMatrix();

protected:
    GUIRenderer& guiRenderer_;
    Texture* texture_;
    vec4 color_;
    vec2 offset_;
    mat4 transformMatrix_;
};

std::optional<uint32> GuiRendererElementBase::GetTextureId() const
{
    return texture_ ? texture_->GetGraphicsObjectId() : std::optional<uint32>();
}
const mat4& GuiRendererElementBase::GetTransformMatrix() const
{
    return transformMatrix_;
}
const vec4& GuiRendererElementBase::GetColor() const
{
    return color_;
}
}  // namespace GameEngine
