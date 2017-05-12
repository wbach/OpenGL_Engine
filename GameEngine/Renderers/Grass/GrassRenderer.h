#pragma once
#include "../Renderer.h"
#include "Shaders/GrassShader.h"
#include "../../Objects/RenderAble/Grass.h"

class CGrassRenderer : public CRenderer
{
public:
	CGrassRenderer(SProjection* projection_matrix, std::weak_ptr<CFrameBuffer> framebuffer);
	virtual void Init() override;
	virtual void PrepareFrame(CScene* scene) override;
	virtual void Render(CScene* scene) override;
	virtual void EndFrame(CScene* scene) override;
	virtual void Subscribe(CGameObject* gameObject) override;
private:
	CGrassShader m_Shader;
	SProjection* m_Projection;

	std::list<CGrass*> m_Subscribes;
};