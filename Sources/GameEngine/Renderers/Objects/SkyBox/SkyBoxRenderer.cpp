#include "SkyBoxRenderer.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GameEngine/Renderers/Framebuffer/FrameBuffer.h"
#include "Logger/Log.h"

CSkyBoxRenderer::CSkyBoxRenderer(GameEngine::RendererContext& context)
	: CRenderer(context.defferedFrameBuffer_.get())
	, context_(context)
	, shader(context.graphicsApi_)
	, model(nullptr)
	, dayTexture(nullptr)
	, nightTexture(nullptr)
	, resourceManager(context.graphicsApi_)
{
}

void CSkyBoxRenderer::Init()
{
	shader.Init();
	shader.Start();
	shader.LoadProjectionMatrix(context_.projectionMatrix_->GetProjectionMatrix());
	shader.LoadFogColour(.8f, .8f, .8f);
	shader.LoadBlendFactor(1.f);
	shader.Stop();
	Log("Skybox renderer initialized.");
}

void CSkyBoxRenderer::PrepareToRendering(GameEngine::Scene* scene)
{
	context_.graphicsApi_->DisableCulling();
	shader.Start();
	PrepareShaderBeforeFrameRender(scene);
}

void CSkyBoxRenderer::EndRendering()
{
	context_.graphicsApi_->EnableCulling();
	shader.Stop();
}

void CSkyBoxRenderer::PrepareFrame(GameEngine::Scene* scene)
{
	InitMembers(scene);

	if (!CheckModelIsReadyToRender())
		return;

	PrepareToRendering(scene);

	if (!SetTarget())
		return;

	RenderSkyBoxModel();
	EndRendering();
}

bool CSkyBoxRenderer::SetTarget()
{
	if (target == nullptr)
		return false;

	target->BindToDraw();
	return true;
}

bool CSkyBoxRenderer::CheckModelIsReadyToRender()
{
	return model != nullptr && model->isInOpenGL();
}

void CSkyBoxRenderer::PrepareShaderBeforeFrameRender(GameEngine::Scene *scene)
{
	// TO DO - delta time
	shader.LoadViewMatrix(scene->GetCamera()->GetViewMatrix(), 0.1f, 1500.f);
	shader.LoadBlendFactor(scene->GetDayNightCycle().GetDayNightBlendFactor());
}

void CSkyBoxRenderer::RenderSkyBoxModel()
{
	const auto& meshes = model->GetMeshes();
	for (const auto& mesh : meshes)
		RenderSkyBoxMesh(mesh);
}

void CSkyBoxRenderer::Render(GameEngine::Scene* scene)
{

}

void CSkyBoxRenderer::EndFrame(GameEngine::Scene* scene)
{

}

void CSkyBoxRenderer::Subscribe(CGameObject *gameObject)
{

}

void CSkyBoxRenderer::InitMembers(GameEngine::Scene* scene)
{
	LoadModel(resourceManager);
	CreateDayTextures(resourceManager);
	CreateNightTextures(resourceManager);
}

void CSkyBoxRenderer::LoadModel(CResourceManager& resource_manager)
{
	if (model != nullptr)
		return;

	model = resource_manager.LoadModel("Meshes/SkyBox/cube.obj");
	model->OpenGLLoadingPass();
}

void CSkyBoxRenderer::CreateDayTextures(CResourceManager& resource_manager)
{
	if (dayTexture != nullptr)
		return;

	std::vector<std::string> dayTextures
	{
		"Skybox/TropicalSunnyDay/right.png",
		"Skybox/TropicalSunnyDay/left.png",
		"Skybox/TropicalSunnyDay/top.png",
		"Skybox/TropicalSunnyDay/bottom.png",
		"Skybox/TropicalSunnyDay/back.png",
		"Skybox/TropicalSunnyDay/front.png"
	};

	dayTexture = resource_manager.GetTextureLaoder().LoadCubeMap(dayTextures, false);
	dayTexture->OpenGLLoadingPass();
}

void CSkyBoxRenderer::CreateNightTextures(CResourceManager& resource_manager)
{
	if (nightTexture != nullptr)
		return;

	std::vector<std::string> nightTextures
	{
		"Skybox/Night/right.png",
		"Skybox/Night/left.png",
		"Skybox/Night/top.png",
		"Skybox/Night/bottom.png",
		"Skybox/Night/back.png",
		"Skybox/Night/front.png"
	};

	nightTexture = resource_manager.GetTextureLaoder().LoadCubeMap(nightTextures, false);
	nightTexture->OpenGLLoadingPass();
}

void CSkyBoxRenderer::BindTextures() const
{
	BindCubeMapTexture(dayTexture, 0);
	BindCubeMapTexture(nightTexture, 1);
}

void CSkyBoxRenderer::BindCubeMapTexture(CTexture* texture, int id) const
{
	if (texture == nullptr)
		return;

	context_.graphicsApi_->ActiveTexture(id, texture->GetId());
}

void CSkyBoxRenderer::RenderSkyBoxMesh(const CMesh & mesh) const
{
	if (!mesh.IsInit())
		return;

	BindTextures();
	context_.graphicsApi_->RenderMesh(mesh.GetObjectId());
}
