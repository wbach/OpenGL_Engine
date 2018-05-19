#include "SimpleRenderer.h"
#include "GameEngine/Objects/RenderAble/Entity/Entity.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
SimpleRenderer::SimpleRenderer(IGraphicsApiPtr api, CProjection* projection_matrix, std::function<void(RendererFunctionType, RendererFunction)> rendererFunction)
    : graphicsApi_(api)
	, shader(api)
	, projectionMatrix(projection_matrix)
{
}

void SimpleRenderer::Init()
{
    shader.Init();
    shader.Start();
    shader.LoadProjectionMatrix(projectionMatrix->GetProjectionMatrix());
    shader.Stop();
}

void SimpleRenderer::PrepareFrame(Scene* scene)
{
    shader.Start();
    shader.LoadViewMatrix(scene->GetCamera()->GetViewMatrix());
}

void SimpleRenderer::Render(Scene* scene)
{
    for (auto& entity : subscribes)
	{
		if (entity->GetModel(LevelOfDetail::L1) == nullptr)
			continue;

        RenderModel(entity->GetModel(LevelOfDetail::L1), entity->worldTransform.GetMatrix());
	}
}

void SimpleRenderer::EndFrame(Scene* scene)
{
    shader.Stop();
}

void SimpleRenderer::Subscribe(CGameObject * gameObject)
{
	auto entity = dynamic_cast<CEntity*>(gameObject);
	if (entity != nullptr)
        subscribes.push_back(entity);
}

void SimpleRenderer::UnSubscribe(CGameObject* gameObject)
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

void SimpleRenderer::ReloadShaders()
{
	shader.Stop();
	shader.Reload();
	Init();
}

void SimpleRenderer::RenderModel(CModel * model, const mat4 & transform_matrix) const
{
    shader.LoadTransformMatrix(transform_matrix);

	for (const auto& mesh : model->GetMeshes())
	{
		BindTextures(mesh.GetMaterial());
		graphicsApi_->RenderMesh(mesh.GetObjectId());
	}
}

void SimpleRenderer::BindTextures(const SMaterial & material) const
{
    if (material.diffuseTexture != nullptr)
	{
		graphicsApi_->ActiveTexture(0, material.diffuseTexture->GetId());
	}
}
} // GameEngine
