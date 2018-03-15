#include "SkyBoxRenderer.h"
#include "../Framebuffer/FrameBuffer.h"
#include "../../Resources/Models/Model.h"

#include "../../Resources/Textures/Texture.h"
#include "../../Renderers/Projection.h"
#include "../../Scene/Scene.hpp"
#include "OpenGL/OpenGLUtils.h"
#include "Logger/Log.h"

CSkyBoxRenderer::CSkyBoxRenderer(CProjection *projection_matrix, CFrameBuffer* framebuffer)
	: CRenderer(framebuffer)
	, model(nullptr)
	, dayTexture(nullptr)
	, nightTexture(nullptr)
	, projectionMatrix(projection_matrix)
{
}

void CSkyBoxRenderer::Init()
{
	shader.Init();
	shader.Start();
	shader.LoadProjectionMatrix(projectionMatrix->GetProjectionMatrix());
	shader.LoadFogColour(.8f, .8f, .8f);
	shader.LoadBlendFactor(1.f);
	shader.Stop();
	Log("Skybox renderer initialized.");
}

void CSkyBoxRenderer::PrepareToRendering(GameEngine::Scene* scene)
{
	Utils::DisableCulling();
	shader.Start();
	PrepareShaderBeforeFrameRender(scene);	
}

void CSkyBoxRenderer::EndRendering()
{
	Utils::EnableCulling();
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

	dayTexture = LoadCubeMapTexture(resource_manager, dayTextures);
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

	nightTexture = LoadCubeMapTexture(resource_manager, nightTextures);
	nightTexture->OpenGLLoadingPass();
}

CTexture* CSkyBoxRenderer::LoadCubeMapTexture(CResourceManager& resource_manager, std::vector<std::string> textures_files)
{
	return resource_manager.GetTextureLaoder().LoadCubeMap(textures_files, false);
}

void CSkyBoxRenderer::BindTextures(const SMaterial & material) const
{
	BindCubeMapTexture(dayTexture, 0);
	BindCubeMapTexture(nightTexture, 1);
}

void CSkyBoxRenderer::BindCubeMapTexture(CTexture* texture, int id) const
{
	if (texture == nullptr)
		return;

	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture->GetId());
}

void CSkyBoxRenderer::RenderSkyBoxMesh(const CMesh & mesh) const
{
	if (!mesh.IsInit())
		return;

    Utils::EnableVao ev(mesh.GetVao(), mesh.GetUsedAttributes());
	BindTextures(mesh.GetMaterial());
	glDrawElements(GL_TRIANGLES, mesh.GetVertexCount(), GL_UNSIGNED_SHORT, 0);
}
