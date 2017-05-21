#include "SkyBoxRenderer.h"
#include "../Framebuffer/FrameBuffer.h"
#include "../../Utils/OpenGL/OpenGLUtils.h"
#include "../../Resources/Models/Model.h"
#include "../../Resources/Textures/Texture.h"
#include "../../Engine/Projection.h"
#include "../../Scene/Scene.hpp"

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

void CSkyBoxRenderer::PrepareFrame(CScene *scene)
{
	InitMembers(scene);

	Utils::DisableCulling();
   shader.Start();
   shader.LoadViewMatrix(scene->GetCamera()->GetViewMatrix(), scene->GetDeltaTime(), 500.f);
   shader.LoadBlendFactor(scene->GetDayNightCycle().GetDayNightBlendFactor());

   if (target == nullptr)
       return;

   target->BindToDraw();

   for(const auto& mesh : model->GetMeshes())
   {
	   Utils::EnableVao(mesh.GetVao(), mesh.GetUsedAttributes());
	   BindTextures(mesh.GetMaterial());
       glDrawElements(GL_TRIANGLES, mesh.GetVertexCount(), GL_UNSIGNED_SHORT, 0);
	   Utils::DisableVao(mesh.GetUsedAttributes());
   }
   Utils::EnableCulling();
   shader.Stop();
}

void CSkyBoxRenderer::Render(CScene *scene)
{

}

void CSkyBoxRenderer::EndFrame(CScene *scene)
{

}

void CSkyBoxRenderer::Subscribe(CGameObject *gameObject)
{

}

void CSkyBoxRenderer::InitMembers(CScene* scene)
{
	if (model == nullptr)
	{
		model = scene->GetResourceManager().LoadModel("../Data/Meshes/SkyBox/cube.obj");
		model->OpenGLLoadingPass();
	}
	if (dayTexture == nullptr)
	{
		std::vector<std::string> dayTextures
		{
			"../Data/Skybox/TropicalSunnyDay/right.png",
			"../Data/Skybox/TropicalSunnyDay/left.png",
			"../Data/Skybox/TropicalSunnyDay/top.png",
			"../Data/Skybox/TropicalSunnyDay/bottom.png",
			"../Data/Skybox/TropicalSunnyDay/back.png",			
			"../Data/Skybox/TropicalSunnyDay/front.png"			
		};
		dayTexture = scene->GetResourceManager().GetTextureLaoder().LoadCubeMap(dayTextures, false);
		dayTexture->OpenGLLoadingPass();
	}
	if (nightTexture == nullptr)
	{
		std::vector<std::string> nightTextures
		{
			"../Data/Skybox/Night/right.png",
			"../Data/Skybox/Night/left.png",
			"../Data/Skybox/Night/top.png",
			"../Data/Skybox/Night/bottom.png",
			"../Data/Skybox/Night/back.png",
			"../Data/Skybox/Night/front.png"
		};
		nightTexture = scene->GetResourceManager().GetTextureLaoder().LoadCubeMap(nightTextures, false);
		nightTexture->OpenGLLoadingPass();
	}
}

void CSkyBoxRenderer::BindTextures(const SMaterial & material) const
{
	if (dayTexture != nullptr)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, dayTexture->GetId());
	}

	if (nightTexture != nullptr)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, nightTexture->GetId());
	}
}
