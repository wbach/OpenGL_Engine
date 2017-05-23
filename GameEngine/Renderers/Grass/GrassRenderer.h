#pragma once
#include "../Renderer.h"
#include "Shaders/GrassShader.h"

struct SGrass;
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
    CGrassShader shader;
    CProjection* projection;

    std::list<SGrass*> subscribes;
	float viewDistance = 30.f;
};
