#pragma once
#include "../../Renderer.h"
#include "Shaders/ClassicTerrainShader.h"
#include <list>

class CModel;
class CTerrain;
class CProjection;

class ClassicTerrainRenderer : public CRenderer
{
public:
	ClassicTerrainRenderer(CProjection* projection_matrix, CFrameBuffer* framebuffer);
	// Loading lights itp to shader
	virtual void Init() override;
	virtual void PrepareFrame(GameEngine::Scene* scene) override;
	virtual void Render(GameEngine::Scene* scene) override;
	virtual void EndFrame(GameEngine::Scene* scene) override;
	virtual void Subscribe(CGameObject* gameObject) override;
	virtual void UnSubscribe(CGameObject* gameObject) override;
	void RenderModel(CModel* model, const mat4& transform_matrix) const;

private:
	void BindTextures(CTerrain* terrain) const;

private:
    CTerrainShader  shader;
    CProjection*	projectionMatrix;

    vec4	clipPlane;

    std::list<CTerrain*> subscribes;
};
