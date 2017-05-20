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
	CGrassShader m_Shader;
	CProjection* m_Projection;

	std::list<SGrass*> m_Subscribes;
	float viewDistance = 30.f;
};