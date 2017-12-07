#include "SimpleRenderer.h"
#include "../Objects/RenderAble/Entity/Entity.h"
#include "../Engine/Projection.h"
#include "../Scene/Scene.hpp"

#include "OpenGL/OpenGLUtils.h"

SimpleRenderer::SimpleRenderer(CProjection* projection_matrix)
    : projectionMatrix(projection_matrix)
{
}

void SimpleRenderer::Init()
{
    shader.Init();
    shader.Start();
    assert(projectionMatrix != nullptr);
    shader.LoadProjectionMatrix(projectionMatrix->GetProjectionMatrix());
    shader.Stop();
}

void SimpleRenderer::PrepareFrame(CScene * scene)
{
    shader.Start();
    shader.LoadViewMatrix(scene->GetCamera()->GetViewMatrix());
}

void SimpleRenderer::Render(CScene * scene)
{
    for (auto& entity : subscribes)
	{
		if (entity->GetModel(0) == nullptr)
			continue;

        RenderModel(entity->GetModel(0), entity->worldTransform.GetMatrix());
	}
}

void SimpleRenderer::EndFrame(CScene * scene)
{
    shader.Stop();
}

void SimpleRenderer::Subscribe(CGameObject * gameObject)
{
	auto entity = dynamic_cast<CEntity*>(gameObject);
	if (entity != nullptr)
        subscribes.push_back(entity);
}

void SimpleRenderer::RenderModel(CModel * model, const mat4 & transform_matrix) const
{
    shader.LoadTransformMatrix(transform_matrix);

	for (const auto& mesh : model->GetMeshes())
	{
        Utils::EnableVao ev(mesh.GetVao(), mesh.GetUsedAttributes());
		BindTextures(mesh.GetMaterial());
		glDrawElements(GL_TRIANGLES, mesh.GetVertexCount(), GL_UNSIGNED_SHORT, 0);
	}
}

void SimpleRenderer::BindTextures(const SMaterial & material) const
{
    if (material.diffuseTexture != nullptr)
	{
		glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.diffuseTexture->GetId());
			//TODO : load using texture to shader
			//m_SimpleShader.LOad
	}
}
