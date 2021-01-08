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

    void SetLocalScale(const vec2&) override;
    void SetLocalPostion(const vec2&) override;

    void SetScreenScale(const vec2&) override;
    void SetScreenPostion(const vec2&) override;

    void SetColor(const vec3&);
    void SetColor(const vec4&);
    inline std::optional<uint32> GetTextureId() const;
    inline const vec4& GetColor() const;
    inline const GeneralTexture* GetTexture() const;
    void setParent(GuiElement*) override;
    virtual mat4 GetTransformMatrix() const;

protected:
    IResourceManager& resourceManager_;
    GUIRenderer& guiRenderer_;
    GeneralTexture* texture_;
    vec4 color_;
};
std::optional<uint32> GuiRendererElementBase::GetTextureId() const
{
    return texture_ ? texture_->GetGraphicsObjectId() : std::optional<uint32>();
}
const vec4& GuiRendererElementBase::GetColor() const
{
    return color_;
}
const GeneralTexture* GuiRendererElementBase::GetTexture() const
{
    return texture_;
}
}  // namespace GameEngine
