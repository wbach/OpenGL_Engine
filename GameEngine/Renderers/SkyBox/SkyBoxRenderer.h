#pragma once
#include "../Renderer.h"
#include "Shaders/SkyBoxShader.h"

class CModel;
class CTexture;
class CProjection;
struct SMaterial;

class CSkyBoxRenderer : public CRenderer
{
public:
    CSkyBoxRenderer(CProjection* projection_matrix, CFrameBuffer* framebuffer);
    virtual void Init() override;
    virtual void PrepareFrame(CScene* scene) override;
    virtual void Render(CScene* scene) override;
    virtual void EndFrame(CScene* scene) override;
    virtual void Subscribe(CGameObject* gameObject) override;
private:
	void InitMembers(CScene* scene);
	void BindTextures(const SMaterial& material) const;

private:
    CSkyBoxShader   shader;
    CModel*         model;
    CTexture*		dayTexture;
    CTexture*		nightTexture;

    CProjection*	projectionMatrix;
    glm::vec4       clipPlane;
};
