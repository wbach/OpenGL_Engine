#pragma once
#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"

namespace GameEngine
{
class GUIRenderer;
class IResourceManager;

class GuiRendererElementBase : public GuiElement
{
public:
    GuiRendererElementBase(IResourceManager&, GUIRenderer&, GuiElementTypes);
    ~GuiRendererElementBase() override;

    void SetScale(const vec2&) override;
    void SetPostion(const vec2&) override;

    void SetColor(const vec3&);
    void SetColor(const vec4&);
    inline std::optional<uint32> GetTextureId() const;
    inline const mat4& GetTransformMatrix() const;
    inline const vec4& GetColor() const;
    const GeneralTexture* GetTexture() const;

protected:
    void CalculateMatrix();

protected:
    IResourceManager& resourceManager_;
    GUIRenderer& guiRenderer_;
    GeneralTexture* texture_;
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
inline const GeneralTexture* GuiRendererElementBase::GetTexture() const
{
    return texture_;
}
}  // namespace GameEngine
