#include "TessellationTerrainRenderer.h"
#include "../../Framebuffer/FrameBuffer.h"
#include "../../../Scene/Scene.hpp"
#include "../../../Engine/Projection.h"
#include "../../../Objects/RenderAble/Terrain/Terrain.h"
#include "../../../Utils/GLM/GLMUtils.h"
#include "../../../Utils/EngineUitls.h"

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
    for (auto& sub : subscribes)
	{
        sub->quad.Init();
	}	

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

    Log("Current terrains count to render : " + std::to_string(subscribes.size()) );
    for (auto& sub : subscribes)
    {
        auto position = sub->worldTransform.GetPosition();
       // position *= glm::vec3(1, 1, 100);
        shader.Load(CTesselationTerrainShader::UniformLocation::TransformMatrix, Utils::CreateTransformationMatrix(position, glm::vec3(0, 0, 0), glm::vec3(100)));

		BindTextures(sub);

		if (sub->model != nullptr)
		{
			for (auto& m : sub->model->GetMeshes())
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



    subscribes.push_back(terrain);
}

void CTessellationTerrainRenderer::RenderModel(CModel * model, const glm::mat4 & transform_matrix) const
{
}

void CTessellationTerrainRenderer::BindTextures(TerrainPtr terrain) const
{
	int i = 0;
    for (auto& t : terrain->textures)
	{
		if (t != nullptr)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, t->GetId());
		}
		i++;
    }
}

TerrainPtrs CTessellationTerrainRenderer::GetTerrainsInRange(const glm::vec3& position, float range) const
{
//    TerrainPtrs terrain_list;




}

void CTessellationTerrainRenderer::AllocateTerrainsGrid()
{
    subscribes.resize(gridSize * gridSize);
}

void CTessellationTerrainRenderer::AddTerrainToGrid(TerrainPtr terrain, const wb::vec2i &pos)
{
    int index = pos.x + gridSize*pos.y;

    if(index > subscribes.size())
    {
        ++gridSize;
        AllocateTerrainsGrid();
    }

    subscribes[index] = terrain;
}
