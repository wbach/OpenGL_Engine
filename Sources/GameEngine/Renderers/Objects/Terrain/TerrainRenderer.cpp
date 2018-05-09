#include "TerrainRenderer.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Renderers/Framebuffer/FrameBuffer.h"
#include "GameEngine/Objects/RenderAble/Terrain/Terrain.h"
#include "GameEngine/Renderers/Objects/Shadows/ShadowFrameBuffer.h"
#include "GameEngine/Objects/RenderAble/Terrain/TerrainDef.h"
#include "GameEngine/Objects/RenderAble/Terrain/TerrainWrapper.h"
#include "GLM/GLMUtils.h"

namespace GameEngine
{
	CTerrainRenderer::CTerrainRenderer(IGraphicsApiPtr graphicsApi, CProjection* projection_matrix, CFrameBuffer* framebuffer, RendererContext* shadowRendererContext)
		: CRenderer(framebuffer)
		, graphicsApi_(graphicsApi)
		, shader(graphicsApi)
		, projectionMatrix(projection_matrix)
		, rendererContext_(shadowRendererContext)
		, clipPlane(vec4(0, 1, 0, 100000))
		, objectId(0)
	{
	}
	void CTerrainRenderer::Init()
	{
		InitShader();
		objectId = graphicsApi_->CreatePurePatchMeshInstanced(4, static_cast<uint32>(Terrain::SIZE * Terrain::SIZE));
		Log("CTerrainRenderer initialized.");
	}
	void CTerrainRenderer::PrepareFrame(GameEngine::Scene* scene)
	{
	}
	void CTerrainRenderer::Render(GameEngine::Scene* scene)
	{
		if (target == nullptr)
			return;

		target->BindToDraw();
		shader.Start();
		shader.Load(TerrainShader::lightDirection, scene->GetDirectionalLight().GetDirection());
		shader.Load(TerrainShader::playerPosition, scene->GetCamera()->GetPosition());
		shader.Load(TerrainShader::toShadowMapSpace, rendererContext_->toShadowMapZeroMatrix_);

		auto modelViewMatrix = scene->GetCamera()->GetViewMatrix();
		modelViewMatrix[3][0] = 0;
		modelViewMatrix[3][1] = 0;
		modelViewMatrix[3][2] = 0;

		RenderSubscribers(modelViewMatrix, 2);
		shader.Stop();

	}
	void CTerrainRenderer::RenderSubscribers(const mat4& viewMatrix, int range) const
	{
		for (auto& sub : subscribes)
		{			
			shader.Load(TerrainShader::modelViewMatrix, viewMatrix);
			shader.Load(TerrainShader::modelViewProjectionMatrix, projectionMatrix->GetProjectionMatrix() * viewMatrix);
			RenderSubscriber(sub);
		}
	}
	void CTerrainRenderer::RenderSubscriber(TerrainPtr sub) const
	{
		if (sub == nullptr)
			return;

		BindTextures(sub);
		graphicsApi_->RenderPurePatchedMeshInstances(objectId);
	}
	void CTerrainRenderer::InitShader()
	{
		shader.Init();
		shader.Start();
		shader.Load(TerrainShader::heightFactor, Terrain::HEIGHT_FACTOR);
		shader.Load(TerrainShader::shadowVariables, vec3(1.f, 35.f, 2048.f));
		shader.Load(TerrainShader::projectionMatrix, projectionMatrix->GetProjectionMatrix());
		shader.Stop();
	}
	void CTerrainRenderer::BindTextures(TerrainPtr terrain) const
	{
		graphicsApi_->ActiveTexture(0, rendererContext_->shadowsFrameBuffer_->GetShadowMap());

		const auto& textures = terrain->Get()->textures;
		for (const auto& t : textures)
			BindTexture(t.second, t.first);
	}
	void CTerrainRenderer::BindTexture(CTexture* texture, int id) const
	{
		if (texture == nullptr)
			return;

		graphicsApi_->ActiveTexture(id, texture->GetId());
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
