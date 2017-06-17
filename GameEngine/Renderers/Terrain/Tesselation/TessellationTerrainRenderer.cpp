#include "TessellationTerrainRenderer.h"
#include "../../Framebuffer/FrameBuffer.h"
#include "../../../Scene/Scene.hpp"
#include "../../../Engine/Projection.h"
#include "../../../Objects/RenderAble/Terrain/Terrain.h"
#include "../../../Objects/RenderAble/Terrain/TerrainWrapper.h"

#include "OpenGL/OpenGLUtils.h"
#include "GLM/GLMUtils.h"
#include "EngineUitls.h"

const float heightFactor = 25.f;

CTessellationTerrainRenderer::CTessellationTerrainRenderer(CProjection * projection_matrix, CFrameBuffer* framebuffer)
    : CRenderer(framebuffer)
    , projectionMatrix(projection_matrix)
    , clipPlane(glm::vec4(0, 1, 0, 100000))
{
    AllocateTerrainsGrid();
}

void CTessellationTerrainRenderer::Init()
{
    shader.Init();
    shader.Start();
    assert(projectionMatrix != nullptr);

	GLfloat viewport[4];
	glGetFloatv(GL_VIEWPORT, viewport);
    shader.Load(CTesselationTerrainShader::UniformLocation::Viewport, glm::vec4(viewport[0], viewport[1], viewport[2], viewport[3]));
    //m_Shader.Load(CTerrainShader::UniformLocation::ViewDistance, 500.f);
    shader.Load(CTesselationTerrainShader::UniformLocation::ProjectionMatrix, projectionMatrix->GetProjectionMatrix());
    shader.Load(CTesselationTerrainShader::UniformLocation::HeightFactor, heightFactor);
    shader.Stop();

	Log("CTerrainRenderer initialized.");
}

void CTessellationTerrainRenderer::PrepareFrame(CScene * scene)
{ 
    shader.Start();
    shader.Load(CTesselationTerrainShader::UniformLocation::ViewMatrix, scene->GetCamera()->GetViewMatrix());
	//m_Shader.Load(CTerrainShader::UniformLocation::ScreenSize, glm::vec2(1000, 600));
	
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

	auto subscribes_in_range = GetTerrainsInRange(scene->GetCamera()->GetPosition(), 2);

    //Log("Current terrains count to render : " + std::to_string(subscribes_in_range.size()) );
    for (auto& sub : subscribes_in_range)
    {
		if (sub == nullptr || !sub->Get()->model->isInOpenGL()) continue;

        auto position = sub->worldTransform.GetPosition();
       // position *= glm::vec3(1, 1, 100);
        shader.Load(CTesselationTerrainShader::UniformLocation::TransformMatrix, Utils::CreateTransformationMatrix(position, glm::vec3(0, 0, 0), glm::vec3(100)));

		BindTextures(sub);

		if (sub->Get()->model != nullptr)
		{
			for (auto& m : sub->Get()->model->GetMeshes())
            {
                Utils::EnableVao(m.GetVao(), m.GetUsedAttributes());
                glDrawElements(GL_PATCHES, m.GetVertexCount(), GL_UNSIGNED_SHORT, 0);
                Utils::DisableVao(m.GetUsedAttributes());
			}				
		}
    }
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

void CTessellationTerrainRenderer::RenderModel(CModel * model, const glm::mat4 & transform_matrix) const
{
}

void CTessellationTerrainRenderer::BindTextures(TerrainPtr terrain) const
{
	int i = 0;
    for (auto& t : terrain->Get()->textures)
	{
		if (t != nullptr)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, t->GetId());
		}
		i++;
    }
}

TerrainPtrs CTessellationTerrainRenderer::GetTerrainsInRange(const glm::vec3& position, int range) const
{
   TerrainPtrs terrain_list;

	auto position_in_grid = Utils::CalculatePlaceInGird(position, TERRAIN_SIZE);

	auto edge_min_max_y = Utils::CalcualeteEdgeMinMaxValueInGrid(position_in_grid.y, range, gridSize);
	auto edge_min_max_x = Utils::CalcualeteEdgeMinMaxValueInGrid(position_in_grid.x, range, gridSize);

	/*Log("Position " + Utils::ToString(position));
	Log("Position in grid : " + wb::to_string(position_in_grid));
	Log("Min x: " + wb::to_string(edge_min_max_x));
	Log("Min y: " + wb::to_string(edge_min_max_y));*/

   for(int y = edge_min_max_y.x; y <= edge_min_max_y.y; y++)
	   for (int x = edge_min_max_x.x; x <= edge_min_max_x.y; x++)
	   {
		   auto index = Utils::Calcualte1DindexInArray(x, y, gridSize);
		  // Log("I: " + std::to_string(index));
		   terrain_list.push_back(subscribes[index]);
	   }		   

	return terrain_list;
}

void CTessellationTerrainRenderer::AllocateTerrainsGrid()
{
    subscribes.resize(gridSize * gridSize);
}

void CTessellationTerrainRenderer::AddTerrainToGrid(TerrainPtr terrain, const wb::vec2i &pos)
{
	int index = Utils::Calcualte1DindexInArray(pos, gridSize);

    if(index > static_cast<int>(subscribes.size()))
    {
        ++gridSize;
        AllocateTerrainsGrid();
    }
 //   subscribes[index] = terrain;
}
