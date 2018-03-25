#include "TreeRenderer.h"
#include "Common/Transform.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Projection.h"
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
		shader.Start();
		shader.Load(TreeShader::UniformLocation::ViewMatrix, scene->GetCamera()->GetViewMatrix());
		shader.Load(TreeShader::UniformLocation::CameraPosition, scene->GetCamera()->GetPosition());

		for (auto& sub : subscribes_)
		{
			PreparePositionMap(sub.second);

			const auto& model = sub.second.top->Get(L1);
			const auto& bmodel = sub.second.bottom->Get(L1);

			float factor = model->GetScaleFactor() > bmodel->GetScaleFactor() ? model->GetScaleFactor() : bmodel->GetScaleFactor();
			mat4 normalizeMatrix = glm::scale(vec3(1.f / factor)) * sub.second.transform;

			graphicsApi_->ActiveTexture(0, sub.second.positionTexture);
			shader.Load(TreeShader::UniformLocation::PositionMapSize, sub.second.textureSize.x);
			graphicsApi_->DisableCulling();
			shader.Load(TreeShader::UniformLocation::UseShading, 0.f);
			RenderModel(model, normalizeMatrix);
			graphicsApi_->EnableCulling();
			shader.Load(TreeShader::UniformLocation::UseShading, 1.f);
			RenderModel(bmodel, normalizeMatrix);
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
		sub.textureSize = component->GetPositionSize2d();
		sub.transform = gameObject->worldTransform.GetMatrix();
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

		auto size = sub.textureSize.x * sub.textureSize.y;
		auto size4c = 4 * size;
		float* data = new float[size4c];
		memset(data, 0, size4c);

		for (uint32 j = 0; j< size; j++)
		{
			if (j > sub.positions->size())
				break;

			auto p = (*sub.positions)[j];

			data[j * 4 + 0] = p.x;
			data[j * 4 + 1] = p.y;
			data[j * 4 + 2] = p.z;
			data[j * 4 + 3] = 1.0f;
		}

		//sub.textureSize = 1024;
		sub.positionTexture = graphicsApi_->CreateTexture(TextureType::FLOAT_TEXTURE_4C, TextureFilter::NEAREST, TextureMipmap::NONE, BufferAtachment::NONE, sub.textureSize, data);
		sub.textureInGpu = true;

		delete[] data;
	}
	void TreeRenderer::RenderModel(CModel * model, const mat4& transorm) const
	{
		for (const auto& mesh : model->GetMeshes())
			RenderMesh(mesh, transorm);
	}
	void TreeRenderer::RenderMesh(const CMesh & mesh, const mat4& transform) const
	{
		shader.Load(TreeShader::UniformLocation::NormalizationMatrix, transform);
		BindMaterial(mesh.GetMaterial());
		graphicsApi_->RenderMeshInstanced(mesh.GetObjectId(), 400);
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
