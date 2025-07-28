#pragma once
#include "GraphicsApi/IGraphicsApi.h"
#include "IRenderer.h"
#include "Utils/Time/TimeMeasurer.h"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class Texture;


class LoadingScreenRenderer : public IRenderer
{
public:
    LoadingScreenRenderer(GraphicsApi::IGraphicsApi&, Texture& bgTexture, Texture& circleTexture);
    ~LoadingScreenRenderer();
    void init() override;
    void render() override;
    void reloadShaders() override;
    void cleanUp() override;

private:
    void prepareRender();
    void renderQuad(const GraphicsApi::ID& bufferId, const GraphicsApi::ID& textureId) const;
    void CreateBuffers();

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    GraphicsApi::ID backgroundBufferId_;
    GraphicsApi::ID circleBufferId_;

    ShaderProgram shader_;
    Texture& circleTexture;
    Texture& backgroundTexture;
    mat4 circleMatrix_;
    Utils::Time::CTimeMeasurer timer_;
    bool isInit_;
};
}  // namespace GameEngine
