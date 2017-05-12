#pragma once
#include "Renderer.h"
#include "Entity/Shaders/SimpleEnityShader.h"

class SimpleRenderer : public CRenderer
{
public:
    SimpleRenderer(SProjection* projection_matrix);

	virtual void Init() override;
	virtual void PrepareFrame(CScene* scene) override;
	virtual void Render(CScene* scene) override;
	virtual void EndFrame(CScene* scene) override;
	virtual void Subscribe(CGameObject* gameObject) override;
	void RenderModel(CModel* model, const glm::mat4& transform_matrix) const;
private:
	void BindTextures(const SMaterial& material) const;

	CSimpleEntityShader m_Shader;
	SProjection* m_ProjectionMatrix;

	std::list<CEntity*> m_Subscribes;
};
