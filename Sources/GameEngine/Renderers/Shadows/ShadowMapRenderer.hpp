#pragma once
#include "ShadowBox.h"
#include "../Renderer.h"
#include "../../Scene/Scene.hpp"
#include "Shaders/ShadowShader.h"

class CMesh;
class CEntity;
class CCamera;
class CProjection;
class CShadowFrameBuffer;
struct SMaterial;

class CShadowMapRenderer : public CRenderer
{
public:
    CShadowMapRenderer(CProjection* projection, CShadowFrameBuffer* framebuffer);
    virtual void Init() override;
    virtual void PrepareFrame(GameEngine::Scene* scene) override;
    virtual void Render(GameEngine::Scene* scene) override;
    virtual void EndFrame(GameEngine::Scene* scene) override;
    virtual void Subscribe(CGameObject* gameObject) override;

private:
    void PrepareRender(GameEngine::Scene*);
    void PrepareShader(CCamera*) const;
    void PrepareLightViewMatrix(const glm::vec3& light_direction);
    void RenderSubscribes() const;
    void RenderEntity(CEntity*) const;
    void RenderMesh(const CMesh& mesh, const mat4& transform_matrix) const;
    void BindMaterial(const SMaterial&) const;

private:
    CProjection* projection;
    CShadowFrameBuffer* shadowFrameBuffer;
    CShadowShader shader;
    CShadowBox shadowBox;
    mat4 projectionViewMatrix;
    std::vector<CEntity*> subscribes;
};
