#pragma once
#include <unordered_map>
#include "../GuiElement.h"
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
typedef std::unordered_map<std::string, GuiTextureElement> GuiTextures;

class GuiTexture : public GuiElement
{
public:
    GuiTexture(IGraphicsApi& graphicsApi, IShaderFactory& shaderFactory);
    virtual ~GuiTexture() override;
    virtual void Init() override;
    virtual void Render() override;
    virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;
    GuiTextures guiTextures_;

private:
    void RenderTextureElement(const GuiTextureElement& te);

private:
    IGraphicsApi& graphicsApi_;
    std::unique_ptr<IShaderProgram> shader_;
};
}  // namespace Gui
}  // namespace Renderer
}  // namespace GameEngine