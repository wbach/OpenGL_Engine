#pragma once
#include "../Renderer.h"
#include "Shaders/TerrainShader.h"
#include "../../Utils/OpenGL/OpenGLUtils.h"

class CTerrainRenderer : public CRenderer
{
public:
	CTerrainRenderer(SProjection* projection_matrix, std::weak_ptr<CFrameBuffer> framebuffer);
	// Loading lights itp to shader
	virtual void Init() override;
	virtual void PrepareFrame(CScene* scene) override;
	virtual void Render(CScene* scene) override;
	virtual void EndFrame(CScene* scene) override;
	virtual void Subscribe(CGameObject* gameObject) override;
	void RenderModel(CModel* model, const glm::mat4& transform_matrix) const;
private:
	void BindTextures(CTerrain* terrain) const;


	CTerrainShader m_Shader;
	SProjection*	m_ProjectionMatrix;

	std::list<CTerrain*> m_Subscribes;

	glm::vec4	m_ClipPlane;
};
