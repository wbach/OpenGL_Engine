#pragma once
#include "Shaders/TesselationTerrainShader.h"
#include "../../Renderer.h"

#include "Types.h"

#include <vector>

class CModel;
class CMesh;
class CTexture;
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

private:
	void RenderModel(CModel* model) const;
    void BindTextures(TerrainPtr terrain) const;
	void BindTexture(CTexture*, int id) const;
    void AllocateTerrainsGrid();
    TerrainPtrs GetTerrainsInRange(const vec3& position, int range) const;
	TerrainPtrs AddTerrainsToGrid(const wb::vec2i& position_in_grid, const wb::vec2i& edge_min_max_x, const wb::vec2i& edge_min_max_y) const;
    void AddTerrainToGrid(TerrainPtrs& terrains, int x, int y) const;
	void RenderSubscribers(const vec3& camera_position, int range) const;
	void RenderSubscriber(TerrainPtr) const;
	void RenderTerrainMesh(const CMesh&) const;
	void PrepareShadersBeforeFrame(TerrainPtr) const;
	bool CheckModelExist(TerrainPtr) const;
	void InitShaderFromLocalVariables() const;

private:
    CTesselationTerrainShader  shader;
    CProjection*	projectionMatrix;

    vec4	clipPlane;

    uint32 gridSize = 20;
    TerrainPtrs subscribes;

	//debug
public:
	static float distanceDev;
	static int minTessLevelOuter;
};
