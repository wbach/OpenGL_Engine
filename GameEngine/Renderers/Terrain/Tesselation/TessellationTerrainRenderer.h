#pragma once
#include "Shaders/TesselationTerrainShader.h"
#include "../../Renderer.h"
#include "../../../Utils/Types.h"
#include <vector>

class CModel;
class CTerrainWrapper;
class CProjection;

typedef CTerrainWrapper* TerrainPtr;
typedef std::vector<TerrainPtr> TerrainPtrs;

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
    void BindTextures(TerrainPtr terrain) const;
    TerrainPtrs GetTerrainsInRange(const glm::vec3& position, int range) const;
    void AllocateTerrainsGrid();
    void AddTerrainToGrid(TerrainPtr terrain, const wb::vec2i& pos);

private:
    CTesselationTerrainShader  shader;
    CProjection*	projectionMatrix;

    glm::vec4	clipPlane;

    uint32 gridSize = 20;
    TerrainPtrs subscribes;
};
