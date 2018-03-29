#include "TreeRenderer.h"
#include "Common/Transform.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Projection.h"
#include "../Framebuffer/FrameBuffer.h"
#include "GameEngine/Components/Renderer/TreeRendererComponent.h"

namespace GameEngine
{
	TreeRenderer::TreeRenderer(GameEngine::IGraphicsApiPtr graphicsApi, CProjection * projection_matrix, CFrameBuffer* framebuffer)
		: CRenderer(framebuffer)
		, shader(graphicsApi)
		, graphicsApi_(graphicsApi)
		, projectionMatrix(projection_matrix)
	{
	}
	void TreeRenderer::Init()
	{
		shader.Init();
		shader.Start();
		shader.Load(TreeShader::UniformLocation::ProjectionMatrix, projectionMatrix->GetProjectionMatrix());
		shader.Stop();
	}
	void TreeRenderer::PrepareFrame(GameEngine::Scene* scene)
	{
	}
	void TreeRenderer::Render(GameEngine::Scene* scene)
	{
		for (auto iter = subscribersToInit_.begin(); iter != subscribersToInit_.end();)
		{
			PreparePositionMap(**iter);
			iter = subscribersToInit_.erase(iter);
		}

		target->BindToDraw();
		shader.Start();
		shader.Load(TreeShader::UniformLocation::ViewMatrix, scene->GetCamera()->GetViewMatrix());
		shader.Load(TreeShader::UniformLocation::CameraPosition, scene->GetCamera()->GetPosition());

		for (auto& sub : subscribes_)
		{
			//PreparePositionMap(sub.second);

			const auto& model = sub.second.top->Get(L1);
			const auto& bmodel = sub.second.bottom->Get(L1);

			float factor = model->GetScaleFactor() > bmodel->GetScaleFactor() ? model->GetScaleFactor() : bmodel->GetScaleFactor();
			mat4 normalizeMatrix = glm::scale(vec3(1.f / factor)) * sub.second.transform;

			auto count = sub.second.positions->size();

			graphicsApi_->ActiveTexture(0, sub.second.positionTexture);
			shader.Load(TreeShader::UniformLocation::PositionMapSize, count);
			graphicsApi_->DisableCulling();
			shader.Load(TreeShader::UniformLocation::UseShading, 0.f);
			RenderModel(model, normalizeMatrix, count);
			graphicsApi_->EnableCulling();
			shader.Load(TreeShader::UniformLocation::UseShading, 1.f);
			RenderModel(bmodel, normalizeMatrix, count);
		}
		shader.Stop();
	}
	void TreeRenderer::EndFrame(GameEngine::Scene * scene)
	{
	}
	void TreeRenderer::Subscribe(CGameObject * gameObject)
	{
		if (gameObject == nullptr)
			return;

		auto component = gameObject->GetComponent<Components::TreeRendererComponent>();

		if (component == nullptr)
			return;

		auto& sub = subscribes_[gameObject->GetId()];
		sub.top = &component->GetTopModelWrapper();
		sub.bottom = &component->GetBottomModelWrapper();
		sub.positions = &component->GetPositions();
		sub.transform = gameObject->worldTransform.GetMatrix();
		subscribersToInit_.push_back(&sub);
	}
	void TreeRenderer::UnSubscribe(CGameObject * gameObject)
	{
	}
	void TreeRenderer::UnSubscribeAll()
	{
	}
	void TreeRenderer::ReloadShaders()
	{
		shader.Stop();
		shader.Reload();
		Init();
	}
	void TreeRenderer::PreparePositionMap(Subscriber & sub)
	{
		if (sub.textureInGpu)
			return;

		sub.positionTexture = graphicsApi_->CreateTexture(TextureType::FLOAT_TEXTURE_3C, TextureFilter::NEAREST, TextureMipmap::NONE, BufferAtachment::NONE, vec2ui(sub.positions->size(), 1), &(*sub.positions)[0]);
		sub.textureInGpu = true;
	}
	void TreeRenderer::RenderModel(CModel * model, const mat4& transorm, uint32 size) const
	{
		for (const auto& mesh : model->GetMeshes())
			RenderMesh(mesh, transorm, size);
	}
	void TreeRenderer::RenderMesh(const CMesh & mesh, const mat4& transform, uint32 size) const
	{
		shader.Load(TreeShader::UniformLocation::NormalizationMatrix, transform);
		BindMaterial(mesh.GetMaterial());
		graphicsApi_->RenderMeshInstanced(mesh.GetObjectId(), size);
	}
	void TreeRenderer::RenderTrees()
	{
	}
	void TreeRenderer::BindMaterial(const SMaterial & material) const
	{
		if (material.isTransparency)
			graphicsApi_->DisableCulling();

		shader.Load(TreeShader::UniformLocation::ModelMaterial_Ambient, material.ambient);
		shader.Load(TreeShader::UniformLocation::ModelMaterial_Diffuse, material.diffuse);
		shader.Load(TreeShader::UniformLocation::ModelMaterial_Specular, material.specular);
		shader.Load(TreeShader::UniformLocation::ModelMaterial_ShineDumper, material.shineDamper);

		if (material.diffuseTexture != nullptr && material.diffuseTexture->IsInitialized())
		{
			graphicsApi_->ActiveTexture(1, material.diffuseTexture->GetId());
		}

		if (material.ambientTexture != nullptr && material.ambientTexture->IsInitialized())
		{
			graphicsApi_->ActiveTexture(2, material.ambientTexture->GetId());
		}

		if (material.normalTexture != nullptr && material.normalTexture->IsInitialized())
		{
			graphicsApi_->ActiveTexture(3, material.normalTexture->GetId());
		}

		if (material.specularTexture != nullptr && material.specularTexture->IsInitialized())
			graphicsApi_->ActiveTexture(4, material.specularTexture->GetId());
	}
	void TreeRenderer::UnBindMaterial(const SMaterial & material) const
	{
	}
} // GameEngine
