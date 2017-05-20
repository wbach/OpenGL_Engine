#include "SkyBoxRenderer.h"
#include "../Framebuffer/FrameBuffer.h"
#include "../../Utils/OpenGL/OpenGLUtils.h"
#include "../../Resources/Models/Model.h"
#include "../../Resources/Textures/Texture.h"
#include "../../Engine/Projection.h"
#include "../../Scene/Scene.hpp"

CSkyBoxRenderer::CSkyBoxRenderer(CProjection *projection_matrix, CFrameBuffer* framebuffer)
    : m_Model(nullptr)
	, m_DayTexture(nullptr)
	, m_NightTexture(nullptr)
    , m_ProjectionMatrix(projection_matrix)
    , CRenderer(framebuffer)
{

}

void CSkyBoxRenderer::Init()
{
    m_Shader.Init();
    m_Shader.Start();
    m_Shader.LoadProjectionMatrix(m_ProjectionMatrix->GetProjectionMatrix());
    m_Shader.LoadFogColour(.8f, .8f, .8f);
    m_Shader.LoadBlendFactor(1.f);
    m_Shader.Stop();
	Log("Skybox renderer initialized.");
}

void CSkyBoxRenderer::PrepareFrame(CScene *scene)
{
	InitMembers(scene);

	Utils::DisableCulling();
   m_Shader.Start();
   m_Shader.LoadViewMatrix(scene->GetCamera()->GetViewMatrix(), scene->GetDeltaTime(), 500.f);
   m_Shader.LoadBlendFactor(scene->GetDayNightCycle().GetDayNightBlendFactor());

   if (m_Target == nullptr)
       return;

   m_Target->BindToDraw();

   for(const auto& mesh : m_Model->GetMeshes())
   {
	   Utils::EnableVao(mesh.GetVao(), mesh.GetUsedAttributes());
	   BindTextures(mesh.GetMaterial());
       glDrawElements(GL_TRIANGLES, mesh.GetVertexCount(), GL_UNSIGNED_SHORT, 0);
	   Utils::DisableVao(mesh.GetUsedAttributes());
   }
   Utils::EnableCulling();
   m_Shader.Stop();
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
	if (m_Model == nullptr)
	{
		m_Model = scene->GetResourceManager().LoadModel("../Data/Meshes/SkyBox/cube.obj");
		m_Model->OpenGLLoadingPass();
	}
	if (m_DayTexture == nullptr)
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
		m_DayTexture = scene->GetResourceManager().GetTextureLaoder().LoadCubeMap(dayTextures, false);
		m_DayTexture->OpenGLLoadingPass();
	}
	if (m_NightTexture == nullptr)
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
		m_NightTexture = scene->GetResourceManager().GetTextureLaoder().LoadCubeMap(nightTextures, false);
		m_NightTexture->OpenGLLoadingPass();
	}
}

void CSkyBoxRenderer::BindTextures(const SMaterial & material) const
{
	if (m_DayTexture != nullptr)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_DayTexture->GetId());
	}

	if (m_NightTexture != nullptr)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_NightTexture->GetId());
	}
}
