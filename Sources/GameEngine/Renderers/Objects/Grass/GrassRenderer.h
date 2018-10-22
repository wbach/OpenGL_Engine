#pragma once
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "Shaders/GrassShader.h"

class CMesh;
class CModel;
class CProjection;

namespace GameEngine
{
struct Grass;

class GrassRenderer : public IRenderer
{
public:
    GrassRenderer(RendererContext& context);
    virtual void Init() override;
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void ReloadShaders() override;
    void Render(Scene* scene);

private:
    void InitShader();
    void PrepareRender(Scene* scene);
    void EndRender() const;
    void RenderSubscribes();
    void RenderModel(CModel* model);
    void RenderMesh(const CMesh& mesh);
    void PrepareShader(Scene* scene);

private:
    RendererContext& context_;
    CGrassShader shader;

    std::list<Grass*> subscribes;
    float viewDistance = 30.f;
};
}  // GameEngine
