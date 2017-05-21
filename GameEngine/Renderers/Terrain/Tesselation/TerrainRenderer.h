#pragma once
#include "../../Renderer.h"
#include "Shaders/TerrainShader.h"
#include <list>

class CModel;
struct STerrain;
class CProjection;

class CTessellationTerrainRenderer : public CRenderer
{
public:
    CTessellationTerrainRenderer(CProjection* projection_matrix, CFrameBuffer* framebuffer);
	// Loading lights itp to shader
	virtual void Init() override;
	virtual void PrepareFrame(CScene* scene) override;
	virtual void Render(CScene* scene) override;
	virtual void EndFrame(CScene* scene) override;
	virtual void Subscribe(CGameObject* gameObject) override;
	void RenderModel(CModel* model, const glm::mat4& transform_matrix) const;
private:
	void BindTextures(STerrain* terrain) const;

private:
    CTerrainShader  shader;
    CProjection*	projectionMatrix;

    glm::vec4	clipPlane;

    std::list<STerrain*> subscribes;
};
