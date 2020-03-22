#pragma once
#include "GraphicsApi/IGraphicsApi.h"
#include "IRenderer.h"
#include "Utils/Time/Timer.h"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class Texture;


class LoadingScreenRenderer : public IRenderer
{
public:
    LoadingScreenRenderer(GraphicsApi::IGraphicsApi&, Texture* bgTexture, Texture* circleTexture);
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

    ShaderProgram shader_;
    Texture* circleTexture;
    Texture* backgroundTexture;
    mat4 circleMatrix_;
    Utils::Timer timer_;
};
}  // namespace GameEngine
