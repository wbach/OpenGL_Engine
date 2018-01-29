#include "TessellationTerrainRenderer.h"
#include "../../Framebuffer/FrameBuffer.h"
#include "../../../Scene/Scene.hpp"
#include "../../../Renderers/Projection.h"
#include "../../../Objects/RenderAble/Terrain/Terrain.h"
#include "../../../Objects/RenderAble/Terrain/TerrainWrapper.h"
#include "../../Shadows/ShadowFrameBuffer.h"

#include "OpenGL/OpenGLUtils.h"
#include "GLM/GLMUtils.h"
#include "EngineUitls.h"

float CTessellationTerrainRenderer::distanceDev = 5.f;
int CTessellationTerrainRenderer::minTessLevelOuter = 8;

CTessellationTerrainRenderer::CTessellationTerrainRenderer(CProjection * projection_matrix, CFrameBuffer* framebuffer, CShadowFrameBuffer* shadowFramebuffer)
    : CRenderer(framebuffer)
    , projectionMatrix(projection_matrix)
    , clipPlane(vec4(0, 1, 0, 100000))
    , shadowFramebuffer(shadowFramebuffer)
{
    AllocateTerrainsGrid();
}

void CTessellationTerrainRenderer::Init()
{
    shader.Init();
    shader.Start();
    assert(projectionMatrix != nullptr);
	InitShaderFromLocalVariables();

	Log("CTerrainRenderer initialized.");
}

void CTessellationTerrainRenderer::PrepareFrame(CScene * scene)
{ 
    shader.Start();
    shader.Load(CTesselationTerrainShader::UniformLocation::ViewMatrix, scene->GetCamera()->GetViewMatrix());
	//m_Shader.Load(CTerrainShader::UniformLocation::ScreenSize, vec2(1000, 600));
	
	//m_Shader.Load(CTerrainShader::UniformLocation::LodFactor, 4);
	//m_Shader.LoadClipPlane(m_ClipPlane);
	//m_Shader.LoadShadowValues(0.f, 10.f, 10.f);

    rendererObjectPerFrame = 0;
    rendererVertixesPerFrame = 0;
    shader.Stop();
}

void CTessellationTerrainRenderer::Render(CScene * scene)
{
    if (target == nullptr)
		return;

    target->BindToDraw();
    shader.Start();
	RenderSubscribers(scene->GetCamera()->GetPosition(), 2);
    shader.Stop();
}

void CTessellationTerrainRenderer::RenderSubscribers(const vec3 & camera_position, int range) const
{
	auto subscribes_in_range = GetTerrainsInRange(camera_position, range);
    //Log("Current terrains count to render : " + std::to_string(subscribes_in_range.size()) );
	for (auto& sub : subscribes_in_range)
		RenderSubscriber(sub);
}

void CTessellationTerrainRenderer::RenderSubscriber(TerrainPtr sub) const
{
	if (sub == nullptr || !sub->Get()->model->isInOpenGL())
		return;

	PrepareShadersBeforeFrame(sub);
	BindTextures(sub);

	if (!CheckModelExist(sub))
		return;
	
	RenderModel(sub->Get()->model);
}

void CTessellationTerrainRenderer::RenderTerrainMesh(const CMesh& m) const
{
	shader.Load(CTesselationTerrainShader::UniformLocation::distanceDev, distanceDev);
	shader.Load(CTesselationTerrainShader::UniformLocation::minTessLevelOuter, minTessLevelOuter);

    Utils::EnableVao ev(m.GetVao(), m.GetUsedAttributes());
	glDrawElements(GL_PATCHES, m.GetVertexCount(), GL_UNSIGNED_SHORT, 0);
}

void CTessellationTerrainRenderer::PrepareShadersBeforeFrame(TerrainPtr sub) const
{
	shader.Load(CTesselationTerrainShader::UniformLocation::HeightFactor, sub->Get()->heightFactor);

	auto position = sub->worldTransform.GetPosition();
	shader.Load(CTesselationTerrainShader::UniformLocation::TransformMatrix, Utils::CreateTransformationMatrix(position, vec3(0, 0, 0), vec3(TERRAIN_SIZE / 2.f)));
}

bool CTessellationTerrainRenderer::CheckModelExist(TerrainPtr sub) const
{
	return sub->Get()->model != nullptr;
}

void CTessellationTerrainRenderer::InitShaderFromLocalVariables() const
{
	shader.Start();
	GLfloat viewport[4];
	glGetFloatv(GL_VIEWPORT, viewport);
	shader.Load(CTesselationTerrainShader::UniformLocation::Viewport, vec4(viewport[0], viewport[1], viewport[2], viewport[3]));
	//m_Shader.Load(CTerrainShader::UniformLocation::ViewDistance, 500.f);
	shader.Load(CTesselationTerrainShader::UniformLocation::ProjectionMatrix, projectionMatrix->GetProjectionMatrix());

	shader.Stop();
}

void CTessellationTerrainRenderer::EndFrame(CScene * scene)
{

}

void CTessellationTerrainRenderer::Subscribe(CGameObject * gameObject)
{
    auto terrain = dynamic_cast<TerrainPtr>(gameObject);
    if (terrain == nullptr)
        return;

    auto position_in_grid = Utils::CalculatePlaceInGird(terrain->worldTransform.GetPosition(), TERRAIN_SIZE);
	//Log("Position : " + wb::to_string(position_in_grid));
	auto index = Utils::Calcualte1DindexInArray(position_in_grid, gridSize);
	//Log("Index : " + std::to_string(index));
	subscribes[index] = terrain;
    //subscribes.push_back(terrain);
}

void CTessellationTerrainRenderer::ReloadShaders()
{
	shader.Reload();
	InitShaderFromLocalVariables();
}

void CTessellationTerrainRenderer::RenderModel(CModel * model) const
{
	for (auto& m : model->GetMeshes())
		RenderTerrainMesh(m);
}

void CTessellationTerrainRenderer::BindTextures(TerrainPtr terrain) const
{
	int i = 0;
    for (auto& t : terrain->Get()->textures)
		BindTexture(t, i++);

    Utils::ActiveBindTexture(i, shadowFramebuffer->GetShadowMap());
}

void CTessellationTerrainRenderer::BindTexture(CTexture* texture, int id) const
{
	if (texture == nullptr)
		return;
	
	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(GL_TEXTURE_2D, texture->GetId());	
}


TerrainPtrs CTessellationTerrainRenderer::GetTerrainsInRange(const vec3& position, int range) const
{
	auto position_in_grid = Utils::CalculatePlaceInGird(position, TERRAIN_SIZE);
	auto edge_min_max_y = Utils::CalcualeteEdgeMinMaxValueInGrid(position_in_grid.y, range, gridSize);
	auto edge_min_max_x = Utils::CalcualeteEdgeMinMaxValueInGrid(position_in_grid.x, range, gridSize);

	/*Log("Position " + Utils::ToString(position));
	Log("Position in grid : " + wb::to_string(position_in_grid));
	Log("Min x: " + wb::to_string(edge_min_max_x));
	Log("Min y: " + wb::to_string(edge_min_max_y));*/
	   
	return AddTerrainsToGrid(position_in_grid, edge_min_max_x, edge_min_max_y);
}

void CTessellationTerrainRenderer::AllocateTerrainsGrid()
{
    subscribes.resize(gridSize * gridSize);
}

TerrainPtrs CTessellationTerrainRenderer::AddTerrainsToGrid(const wb::vec2i & position_in_grid, const wb::vec2i & edge_min_max_x, const wb::vec2i & edge_min_max_y) const
{
	TerrainPtrs terrain_list;
	for (int y = edge_min_max_y.x; y <= edge_min_max_y.y; y++)
		for (int x = edge_min_max_x.x; x <= edge_min_max_x.y; x++)
			AddTerrainToGrid(terrain_list, x, y);
	return terrain_list;
}

void CTessellationTerrainRenderer::AddTerrainToGrid(TerrainPtrs& terrain_list, int x, int y) const
{
	auto index = Utils::Calcualte1DindexInArray(x, y, gridSize);
	// Log("I: " + std::to_string(index));
	terrain_list.push_back(subscribes[index]);
}
