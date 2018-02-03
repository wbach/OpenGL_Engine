#pragma once
#include "Renderer.h"
#include "Entity/Shaders/SimpleEnityShader.h"

class CModel;
class CEntity;
struct SMaterial;
class CProjection;

class SimpleRenderer : public CRenderer
{
public:
    SimpleRenderer(CProjection* projection_matrix);

	virtual void Init() override;
	virtual void PrepareFrame(GameEngine::Scene* scene) override;
	virtual void Render(GameEngine::Scene* scene) override;
	virtual void EndFrame(GameEngine::Scene* scene) override;
	virtual void Subscribe(CGameObject* gameObject) override;
	void RenderModel(CModel* model, const mat4& transform_matrix) const;

private:
	void BindTextures(const SMaterial& material) const;

private:
    CSimpleEntityShader shader;
    CProjection* projectionMatrix;

    std::list<CEntity*> subscribes;
};
