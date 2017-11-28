#pragma once
#include "ShadowBox.h"
#include "../Renderer.h"
#include "Scene/Scene.hpp"
#include "Shaders/ShadowShader.h"

class CCamera;
class CProjection;
class CShadowFrameBuffer;

class CShadowMapRenderer : public CRenderer
{
public:
    CShadowMapRenderer(CProjection *projection, CShadowFrameBuffer *framebuffer);
    virtual void Init() override;
    virtual void PrepareFrame(CScene* scene) override;
    virtual void Render(CScene* scene) override;
    virtual void EndFrame(CScene* scene) override;
    virtual void Subscribe(CGameObject* gameObject) override;

private:
    void Prepare(const glm::vec3& light_direction);

private:
    CProjection* projection;
    CShadowFrameBuffer* shadowFrameBuffer;
    CShadowShader shader;
    CShadowBox shadowBox;
    mat4 projectionViewMatrix;
};
