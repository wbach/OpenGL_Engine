#pragma once
#include "GameEngine/Renderers/GUI/GuiElement.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"

namespace GameEngine
{
class GUIRenderer;
class IResourceManager;

class ENGINE_API GuiRendererElementBase : public GuiElement
{
public:
    GuiRendererElementBase(IResourceManager&, GUIRenderer&, GuiElementTypes);
    ~GuiRendererElementBase() override;

    void SetLocalScale(const vec2&) override;
    void SetLocalPosition(const vec2&) override;

    void SetScreenScale(const vec2&) override;
    void SetScreenPostion(const vec2&) override;

    void SetColor(const Color&);
    virtual std::optional<uint32> GetTextureId() const;
    const Color& GetColor() const;
    const GeneralTexture* GetTexture() const;
    void setParent(GuiElement*) override;
    virtual mat4 GetTransformMatrix() const;

protected:
    IResourceManager& resourceManager_;
    GUIRenderer& guiRenderer_;
    GeneralTexture* texture_;
    Color color_;
};
}  // namespace GameEngine
