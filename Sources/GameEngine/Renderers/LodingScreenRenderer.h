#pragma once
#include "../Shaders/Loading/LoadingShader.h"
#include "IRenderer.h"


namespace GameEngine
{
class Texture;

class LoadingScreenRenderer : public IRenderer
{
public:
    LoadingScreenRenderer(IGraphicsApiPtr, Texture* bgTexture, Texture* circleTexture);
    virtual void Init() override;
    void Render(Scene*);
    virtual void ReloadShaders() override
    {
    }

private:
    void prepareRender();
    void renderQuad(const glm::mat4& transformMatrix, uint32 textureId) const;

private:
    IGraphicsApiPtr graphicsApi_;
    LoadingShader loadingShader;
    Texture* circleTexture;
    Texture* backgroundTexture;
    mat4 transformationMatrix;
};
}  // GameEngine
