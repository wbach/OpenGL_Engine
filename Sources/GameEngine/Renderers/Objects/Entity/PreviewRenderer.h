#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class Texture;
struct Material;
struct RendererContext;

namespace Components
{
class PreviewComponent;
}

class PreviewRenderer : public IRenderer
{
public:
    PreviewRenderer(RendererContext&);
    ~PreviewRenderer();

    void init() override;
    void subscribe(GameObject&) override;
    void unSubscribe(uint32);
    void unSubscribe(GameObject&) override;
    void unSubscribeAll() override;
    void reloadShaders() override;
    void prepare() override;

private:
    void bindMaterial(const Material&) const;
    void unBindMaterial(const Material&) const;
    void bindMaterialTexture(uint32, Texture*, bool) const;

private:
    RendererContext& context_;
    ShaderProgram shader_;
    std::unordered_map<uint32, Components::PreviewComponent*> subcribers_;

    GraphicsApi::ID perFrameBufferId_;
    GraphicsApi::IFrameBuffer* frameBuffer_;
    bool isInit_;
};
}  // namespace GameEngine
