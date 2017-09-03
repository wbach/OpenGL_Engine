#include "TerrainRenderer.h"
#include "../../Framebuffer/FrameBuffer.h"
#include "../../../Scene/Scene.hpp"
#include "../../../Engine/Projection.h"
#include "../../../Objects/RenderAble/Terrain/Terrain.h"

#include "OpenGL/OpenGLUtils.h"
#include "GLM/GLMUtils.h"

const float heightFactor = 25.f;

CTerrainRenderer::CTerrainRenderer(CProjection * projection_matrix, CFrameBuffer* framebuffer)
    : CRenderer(framebuffer)
    , projectionMatrix(projection_matrix)
    , clipPlane(vec4(0, 1, 0, 100000))
{
}

void CTerrainRenderer::Init()
{
    shader.Init();
    shader.Start();
    assert(projectionMatrix != nullptr);

    GLfloat viewport[4];
	glGetFloatv(GL_VIEWPORT, viewport);
    shader.Load(CTerrainShader::UniformLocation::Viewport, vec4(viewport[0], viewport[1], viewport[2], viewport[3]));
    //m_Shader.Load(CTerrainShader::UniformLocation::ViewDistance, 500.f);
    shader.Load(CTerrainShader::UniformLocation::ProjectionMatrix, projectionMatrix->GetProjectionMatrix());
    shader.Load(CTerrainShader::UniformLocation::HeightFactor, heightFactor);
    shader.Stop();

	Log("CTerrainRenderer initialized.");
}

void CTerrainRenderer::PrepareFrame(CScene * scene)
{
    shader.Start();
    shader.Load(CTerrainShader::UniformLocation::ViewMatrix, scene->GetCamera()->GetViewMatrix());
	//m_Shader.Load(CTerrainShader::UniformLocation::ScreenSize, vec2(1000, 600));
	
	//m_Shader.Load(CTerrainShader::UniformLocation::LodFactor, 4);
	//m_Shader.LoadClipPlane(m_ClipPlane);
	//m_Shader.LoadShadowValues(0.f, 10.f, 10.f);

    rendererObjectPerFrame = 0;
    rendererVertixesPerFrame = 0;
    shader.Stop();
}

void CTerrainRenderer::Render(CScene * scene)
{
    if (target == nullptr)
		return;

    target->BindToDraw();
    shader.Start();

    for (auto& sub : subscribes)
	{
        auto position = sub->worldTransform.GetPosition();
        //position *= vec3(100, 0, 100);
        position = vec3(0,100, 0);
        shader.Load(CTerrainShader::UniformLocation::TransformMatrix, Utils::CreateTransformationMatrix(position, vec3(0, 0, 0), vec3(100)));

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

void CTerrainRenderer::EndFrame(CScene * scene)
{
}

void CTerrainRenderer::Subscribe(CGameObject * gameObject)
{
	auto terrain = dynamic_cast<CTerrain*>(gameObject);
	if (terrain != nullptr)
        subscribes.push_back(terrain);
}

void CTerrainRenderer::RenderModel(CModel * model, const mat4 & transform_matrix) const
{
}

void CTerrainRenderer::BindTextures(CTerrain * terrain) const
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
