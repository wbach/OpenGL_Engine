#include "ShadowMapRenderer.hpp"
#include "ShadowFrameBuffer.h"
#include "Engine/Projection.h"
#include "Camera/Camera.h"
#include "GLM/GLMUtils.h"
#include "math.hpp"

CShadowMapRenderer::CShadowMapRenderer(CProjection *projection, CShadowFrameBuffer *framebuffer)
: shadowBox(projection)
, projection(projection)
, shadowFrameBuffer(framebuffer)
, projectionViewMatrix(1.f)
{
}

void CShadowMapRenderer::Init()
{
    shader.Init();
}

void CShadowMapRenderer::PrepareFrame(CScene *scene)
{

}

void CShadowMapRenderer::Render(CScene *scene)
{
    shadowFrameBuffer->BindFBO();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
    shadowBox.Update(scene->GetCamera());
   // Prepare(light_direction);
}

void CShadowMapRenderer::EndFrame(CScene *scene)
{

}

void CShadowMapRenderer::Subscribe(CGameObject *gameObject)
{

}

void CShadowMapRenderer::Prepare(const glm::vec3& light_direction)
{
    auto projection_matrix = Utils::CreateOrthoProjectionMatrix(shadowBox.GetWidth(), shadowBox.GetHeight(), shadowBox.GetLength());
    auto lightViewMatrix = Utils::CreateLightViewMatrix(light_direction, shadowBox.GetCenter());
    shadowBox.SetLightViewMatrix(lightViewMatrix);
    projectionViewMatrix = projection_matrix * lightViewMatrix;
}

