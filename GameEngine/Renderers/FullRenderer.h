#pragma once
#include "Renderer.h"
#include <vector>
#include <memory>

class CDefferedFrameBuffer;
class CProjection;

class FullRenderer : public CRenderer
{
public:
	FullRenderer(CProjection* projection_matrix);
	// Loading lights itp to shader
	virtual void Init() override;
	virtual void PrepareFrame(CScene* scene) override;
	virtual void Render(CScene* scene) override;
	virtual void EndFrame(CScene* scene) override;
	virtual void Subscribe(CGameObject* gameObject) override;
private:
	CProjection* m_ProjectionMatrix;

	//ShadowMap renderes, etc...
	std::vector<std::unique_ptr<CRenderer>> m_Renderers;
	std::shared_ptr<CDefferedFrameBuffer> m_DefferedFrameBuffer;
};