#pragma once
#include "../Renderer.h"
#include "Shaders/GeometryEntityShader.h"

class CProjection;
class CEntity;
class CModel;

class CEntityRenderer : public CRenderer
{
public:
    CEntityRenderer(CProjection* projection_matrix, CFrameBuffer* framebuffer);
    // Loading lights itp to shader
    virtual void Init() override;
    virtual void PrepareFrame(CScene* scene) override;
    virtual void Render(CScene* scene) override;
    virtual void EndFrame(CScene* scene) override;
    virtual void Subscribe(CGameObject* gameObject) override;
    void RenderModel(CModel* model, const mat4& transform_matrix) const;

private:
    void RenderDynamicsEntities();
    void RenderStaticEntities(const wb::vec2i& index);
    wb::vec2i CalcualteCoorditantes(const vec3& v) const;
    const std::list<CEntity*>& GetEntity(uint32 x, uint32 y) const;
    void BindMaterial(const SMaterial& material) const;
    void UnBindMaterial(const SMaterial& material) const;

private:
    CEntityGeometryPassShader shader;
    CProjection* projectionMatrix;

    vec4 clipPlane;

    std::list<CEntity*> dynamicSubscribes;
    std::vector<std::list<CEntity*>> subscribes;

    const uint32 gridSize     = 1000;
    const uint32 gridCellSize = 250;
};
