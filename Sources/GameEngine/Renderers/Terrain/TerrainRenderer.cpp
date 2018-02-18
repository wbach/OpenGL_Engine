#include "TerrainRenderer.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/Framebuffer/FrameBuffer.h"
#include "GameEngine/Objects/RenderAble/Terrain/Terrain.h"
#include "GameEngine/Objects/RenderAble/Terrain/TerrainDef.h"
#include "GameEngine/Objects/RenderAble/Terrain/TerrainWrapper.h"
#include "GLM/GLMUtils.h"

namespace GameEngine
{
	CTerrainRenderer::CTerrainRenderer(CProjection* projection_matrix, CFrameBuffer* framebuffer, CShadowFrameBuffer* shadowFramebuffer)
		: CRenderer(framebuffer)
		, projectionMatrix(projection_matrix)
		, clipPlane(vec4(0, 1, 0, 100000))
		, shadowFramebuffer(shadowFramebuffer)
	{
	}
	void CTerrainRenderer::Init()
	{
		InitShader();

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glBindVertexArray(0);

		Log("CTerrainRenderer initialized.");
	}
	void CTerrainRenderer::PrepareFrame(GameEngine::Scene* scene)
	{
	}
	void CTerrainRenderer::Render(GameEngine::Scene * scene)
	{
		if (target == nullptr)
			return;

		target->BindToDraw();
		shader.Start();
		shader.Load(TerrainShader::playerPosition, scene->GetCamera()->GetPosition());
		RenderSubscribers(scene->GetCamera()->GetViewMatrix(), 2);
		shader.Stop();

	}
	void CTerrainRenderer::RenderSubscribers(const mat4& viewMatrix, int range) const
	{
		for (auto& sub : subscribes)
		{
			auto modelViewMatrix = viewMatrix;

			modelViewMatrix[3][0] = 0;
			modelViewMatrix[3][1] = 0;
			modelViewMatrix[3][2] = 0;

			shader.Load(TerrainShader::modelViewMatrix, modelViewMatrix);			
			shader.Load(TerrainShader::modelViewProjectionMatrix, projectionMatrix->GetProjectionMatrix() * modelViewMatrix);
			RenderSubscriber(sub);
		}
	}
	void CTerrainRenderer::RenderSubscriber(TerrainPtr sub) const
	{
		if (sub == nullptr || !sub->Get()->model->isInOpenGL())
			return;

		BindTextures(sub);

		glBindVertexArray(vao);
		glDrawArraysInstanced(GL_PATCHES, 0, 4, Terrain::SIZE * Terrain::SIZE);
		glBindVertexArray(0);
	}
	void CTerrainRenderer::InitShader()
	{
		shader.Init();
		shader.Start();
		shader.Load(TerrainShader::heightFactor, Terrain::HEIGHT_FACTOR);
		shader.Load(TerrainShader::projectionMatrix, projectionMatrix->GetProjectionMatrix());
		shader.Stop();
	}
	void CTerrainRenderer::BindTextures(TerrainPtr terrain) const
	{
		int i = 0;
		for (auto& t : terrain->Get()->textures)
			BindTexture(t, i++);
	}
	void CTerrainRenderer::BindTexture(CTexture* texture, int id) const
	{
		if (texture == nullptr)
			return;

		glActiveTexture(GL_TEXTURE0 + id);
		glBindTexture(GL_TEXTURE_2D, texture->GetId());
	}
	void CTerrainRenderer::EndFrame(GameEngine::Scene * scene)
	{
	}
	void CTerrainRenderer::Subscribe(CGameObject* gameObject)
	{
		auto terrain = dynamic_cast<TerrainPtr>(gameObject);
		if (terrain == nullptr)
			return;

		subscribes.push_back(terrain);
	}
	void CTerrainRenderer::UnSubscribe(CGameObject * gameObject)
	{
		for (auto iter = subscribes.begin(); iter != subscribes.end(); ++iter)
		{
			if ((*iter)->GetId() == gameObject->GetId())
			{
				subscribes.erase(iter);
				return;
			}
		}
	}
	void CTerrainRenderer::ReloadShaders()
	{
		shader.Reload();
		InitShader();
	}
}
