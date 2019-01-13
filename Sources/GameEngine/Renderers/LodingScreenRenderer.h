#pragma once
#include "IRenderer.h"
#include "GameEngine/Api/IGraphicsApi.h"

namespace GameEngine
{
class Texture;
class IShaderProgram;
class IShaderFactory;

class LoadingScreenRenderer : public IRenderer
{
public:
    LoadingScreenRenderer(IGraphicsApi&, Texture* bgTexture, Texture* circleTexture, IShaderFactory& shaderFactory);
    ~LoadingScreenRenderer();
    virtual void Init() override;
    void Render(Scene*);
    virtual void ReloadShaders() override;

private:
    void prepareRender();
    void renderQuad(const glm::mat4& transformMatrix, uint32 textureId) const;

private:
    IGraphicsApi& graphicsApi_;
    std::unique_ptr<IShaderProgram> shader_;
    Texture* circleTexture;
    Texture* backgroundTexture;
    mat4 transformationMatrix;
};
}  // GameEngine
