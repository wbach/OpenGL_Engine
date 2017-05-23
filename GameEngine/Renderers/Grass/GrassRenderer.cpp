#include "GrassRenderer.h"

#include "../Framebuffer/FrameBuffer.h"

#include "../../Scene/Scene.hpp"
#include "../../Engine/Projection.h"
#include "../../Engine/Configuration.h"
#include "../../Objects/RenderAble/Flora/Grass/Grass.h"
#include "../../Utils/OpenGL/OpenGLUtils.h"

CGrassRenderer::CGrassRenderer(CProjection * projection_matrix, CFrameBuffer* framebuffer)
    : CRenderer(framebuffer)
    , projection(projection_matrix)
{	
}

void CGrassRenderer::Init()
{	
	shader.Init();
	shader.Start();
	shader.LoadProjectionMatrix(projection->GetProjectionMatrix());
	shader.Stop();

	viewDistance = SConfiguration::Instance().floraViewDistance;
	Log("Grass renderer initialized.");
}

void CGrassRenderer::PrepareFrame(CScene * scene)
{		
}

void CGrassRenderer::Render(CScene * scene)
{
	if (target == nullptr)
		return;
	target->BindToDraw();

	shader.Start();
	shader.LoadGlobalTime(scene->GetGlobalTime());
	shader.LoadShadowValues(0.f, 0.f, 512.f);
	shader.LoadViewMatrix(scene->GetCamera()->GetViewMatrix());
    shader.LoadViewDistance(viewDistance);

	Utils::DisableCulling();

	glActiveTexture(GL_TEXTURE0);	

	for (const auto& s : subscribes)
	{
		if (s->model == nullptr)
			continue;

		for (const auto& mesh : s->model->GetMeshes())
		{
            if (mesh.GetMaterial().diffuseTexture == nullptr)
				continue;

            glBindTexture(GL_TEXTURE_2D, mesh.GetMaterial().diffuseTexture->GetId());
			glBindVertexArray(mesh.GetVao());
			glEnableVertexAttribArray(0);
			glDrawArrays(GL_POINTS, 0, mesh.GetVertexCount());
			glDisableVertexAttribArray(0);
			glBindVertexArray(0);
		}
	}
	Utils::EnableCulling();
	
	shader.Stop();
}

void CGrassRenderer::EndFrame(CScene * scene)
{
}

void CGrassRenderer::Subscribe(CGameObject * gameObject)
{
	auto grass = dynamic_cast<SGrass*>(gameObject);
	if (grass != nullptr)
		subscribes.push_back(grass);
}
