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
		std::vector<mat4> transforms;
		transforms.resize(100);

		for (uint32 j = 0; j < 10; j++)
		for (uint32 i = 0; i < 10; i++)
			transforms[i + i*j] = Utils::CreateTransformationMatrix(vec3(350 + 10 * j, -0.5, 500 + 10 * i), vec3(0), vec3(1));

		shader.LoadTransforms(transforms);
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
			if (!sub.second.textureInGpu)
			{
				vec2i size(1024, 1024);

				float* data = new float[4 * size.x * size.y];
				memset(data, 0, 4 * size.x * size.y);

				for (uint32 j = 0; j< size.x * size.y; j++)
				{
					if (j > sub.second.positions->size())
						break;

					auto p = (*sub.second.positions)[j];

					data[j * 4 + 0] = p.x / 1000.f;
					data[j * 4 + 1] = p.y / 1000.f;
					data[j * 4 + 2] = p.z / 1000.f;
					data[j * 4 + 3] = 1.0f;
				}

				sub.second.textureSize = 1024;

				sub.second.positionTexture = graphicsApi_->CreateTexture(TextureType::FLOAT_TEXTURE_4C, TextureFilter::NEAREST, TextureMipmap::NONE, BufferAtachment::NONE, size, data);

				delete[] data;
				sub.second.textureInGpu = true;
			}
			const auto& model = sub.second.top->Get(L1);
			const auto& bmodel = sub.second.bottom->Get(L1);

			float factor = model->GetScaleFactor() > bmodel->GetScaleFactor() ? model->GetScaleFactor() : bmodel->GetScaleFactor();
			mat4 normalizeMatrix = glm::scale(vec3(1.f / factor)) * sub.second.transform;

			graphicsApi_->ActiveTexture(0, sub.second.positionTexture);
			shader.Load(TreeShader::UniformLocation::PositionMapSize, sub.second.textureSize);
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
	void TreeRenderer::RenderModel(CModel * model, const mat4& transorm) const
	{
		for (const auto& mesh : model->GetMeshes())
			RenderMesh(mesh, transorm);
	}
	void TreeRenderer::RenderMesh(const CMesh & mesh, const mat4& transform) const
	{
		shader.Load(TreeShader::UniformLocation::NormalizationMatrix, transform);
		BindMaterial(mesh.GetMaterial());
		graphicsApi_->RenderMeshInstanced(mesh.GetObjectId(), 100);
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
