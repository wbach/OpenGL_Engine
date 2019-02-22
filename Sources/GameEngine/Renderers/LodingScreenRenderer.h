#pragma once
#include "GraphicsApi/IGraphicsApi.h"
#include "IRenderer.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"

namespace GameEngine
{
class Texture;
class IShaderProgram;
class IShaderFactory;

class LoadingScreenRenderer : public IRenderer
{
public:
    LoadingScreenRenderer(GraphicsApi::IGraphicsApi&, Texture* bgTexture, Texture* circleTexture,
                          IShaderFactory& shaderFactory);
    ~LoadingScreenRenderer();
    virtual void Init() override;
    void Render(Scene*);
    virtual void ReloadShaders() override;

private:
    void prepareRender();
    void renderQuad(const GraphicsApi::ID& bufferId, uint32 textureId) const;
    void CreateBuffers();

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    GraphicsApi::ID backgroundBufferId_;
    GraphicsApi::ID circleBufferId_;

    std::unique_ptr<IShaderProgram> shader_;
    Texture* circleTexture;
    Texture* backgroundTexture;
    PerObjectUpdate circleBuffer_;
};
}  // namespace GameEngine
