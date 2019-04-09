#pragma once
#include <unordered_map>
#include "../IGuiRenderer.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GuiTextureElement.h"
#include "GuiTextureShader.h"

namespace GameEngine
{
class IShaderProgram;
class IShaderFactory;

namespace Renderer
{
namespace Gui
{
class GuiTextureRenderer : public IGuiRenderer
{
public:
    GuiTextureRenderer(GraphicsApi::IGraphicsApi& graphicsApi, IShaderFactory& shaderFactory);
    virtual ~GuiTextureRenderer() override;
    virtual void Init() override;
    virtual void Render() override;
    virtual void Subscribe(GuiElement*) override;
    virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;

private:
    void RenderTextureElement(const GuiTextureElement& te);

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    IShaderFactory& shaderFactory_;
    std::unique_ptr<IShaderProgram> shader_;
    std::vector<GuiTextureElement*> textures_;
    uint32 transformBuffer_;
    uint32 textBuffer_;
    bool isInit_;
};
}  // namespace Gui
}  // namespace Renderer
}  // namespace GameEngine