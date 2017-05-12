#pragma once
#include "../Renderer.h"
#include "Shaders/GeometryEntityShader.h"
#include "../../Utils/OpenGL/OpenGLUtils.h"
#include "../Framebuffer/DeferedFrameBuffer/DeferedFrameBuffer.h"

class CEntityRenderer : public CRenderer
{
public:
    CEntityRenderer(SProjection* projection_matrix, std::weak_ptr<CFrameBuffer> framebuffer);
	// Loading lights itp to shader
	virtual void Init() override;
	virtual void PrepareFrame(CScene* scene) override;
	virtual void Render(CScene* scene) override;
	virtual void EndFrame(CScene* scene) override;
	virtual void Subscribe(CGameObject* gameObject) override;
	void RenderModel(CModel* model, const glm::mat4& transform_matrix) const;
	
private:
	wb::vec2i CalcualteCoorditantes(const glm::vec3& v) const;
	const std::list<CEntity*>& GetEntity(uint x, uint y) const;
	void BindMaterial(const SMaterial& material) const;
	void UnBindMaterial(const SMaterial& material) const;
	CEntityGeometryPassShader m_Shader;
	SProjection* m_ProjectionMatrix;

	glm::vec4	m_ClipPlane;

	std::list<CEntity*> dynamicSubscribes;
	std::vector<std::list<CEntity*>> m_Subscribes;

	const uint gridSize = 1000;
	const uint gridCellSize = 25;
};
