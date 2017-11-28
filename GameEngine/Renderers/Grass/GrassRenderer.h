#pragma once
#include "../Renderer.h"
#include "Shaders/GrassShader.h"

struct SGrass;
class CMesh;
class CModel;
class CProjection;

class CGrassRenderer : public CRenderer
{
public:
    CGrassRenderer(CProjection* projection_matrix, CFrameBuffer* framebuffer);
    virtual void Init() override;
    virtual void PrepareFrame(CScene* scene) override;
    virtual void Render(CScene* scene) override;
    virtual void EndFrame(CScene* scene) override;
    virtual void Subscribe(CGameObject* gameObject) override;

private:
    void PrepareRender(CScene* scene);
    void EndRender() const;
    void RenderSubscribes();
    void RenderModel(CModel* model);
    void RenderMesh(const CMesh& mesh);
    void PrepareShader(CScene* scene);

private:
    CGrassShader shader;
    CProjection* projection;

    std::list<SGrass*> subscribes;
    float viewDistance = 30.f;
};
