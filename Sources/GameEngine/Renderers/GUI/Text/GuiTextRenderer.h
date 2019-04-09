#pragma once
#include <unordered_map>
#include "../IGuiRenderer.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "GuiTextElement.h"
#include "Logger/Log.h"

namespace GameEngine
{
typedef std::vector<GuiTextElement*> GuiTexts;
class IShaderProgram;
class IShaderFactory;

class GuiTextRenderer : public IGuiRenderer
{
public:
    GuiTextRenderer(GraphicsApi::IGraphicsApi& graphicsApi, IShaderFactory& shaderFactory);
    virtual ~GuiTextRenderer() override;
    virtual void Init() override;
    virtual void Render() override;
    virtual void Subscribe(GuiElement*) override;
    virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    std::unique_ptr<IShaderProgram> shader_;
    GuiTexts texts_;

    uint32 transformBuffer_;
    uint32 textBuffer_;
    bool isInit_;
};

}  // namespace GameEngine
