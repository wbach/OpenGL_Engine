#pragma once
#include "../Resources/SimpleModels/Quad.h"
#include "../Shaders/Loading/LoadingShader.h"
#include "Renderer.h"

class CTexture;

class CLoadingScreenRenderer : public CRenderer
{
public:
    CLoadingScreenRenderer(CTexture* bgTexture, CTexture* circleTexture);
    virtual void Init() override;
    virtual void PrepareFrame(GameEngine::Scene*) override{}
    virtual void Render(GameEngine::Scene*) override;
    virtual void EndFrame(GameEngine::Scene*) override{}

private:
    void prepareRender();
    void renderQuad(const glm::mat4& transformMatrix, uint32 textureId) const;

private:
    CLoadingShader loadingShader;
    SSimpleQuad quad;
    CTexture* circleTexture;
    CTexture* backgroundTexture;
    mat4 transformationMatrix;
};
