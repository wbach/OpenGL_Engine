#pragma once
#include "../Renderer.h"
#include "Shaders/GeometryEntityShader.h"

class CProjection;
class CEntity;
class CModel;
class CMesh;

namespace GameEngine
{
	class ModelWrapper;
}

struct Subscriber
{
	CGameObject* gameObject;
	GameEngine::ModelWrapper* model;
};

typedef std::unordered_map<uint32_t, Subscriber> SubscribersMap;

class CEntityRenderer : public CRenderer
{
public:
    CEntityRenderer(CProjection* projection_matrix, CFrameBuffer* framebuffer);
    // Loading lights itp to shader
    virtual void Init() override;
    virtual void PrepareFrame(GameEngine::Scene* scene) override;
    virtual void Render(GameEngine::Scene* scene) override;
    virtual void EndFrame(GameEngine::Scene* scene) override;
    virtual void Subscribe(CGameObject* gameObject) override;
	virtual void UnSubscribe(CGameObject* gameObject) override;
	virtual void UnSubscribeAll() override;
	virtual void ReloadShaders() override;

private:
    void RenderModel(CModel* model, const mat4& transform_matrix) const;
    void RenderMesh(const CMesh& mesh, const mat4& transform_matrix) const;
    void RenderDynamicsEntities();
    void RenderStaticEntities(const wb::vec2i& index);
    wb::vec2i CalcualteCoorditantes(const vec3& v) const;
    const SubscribersMap& GetEntity(uint32 x, uint32 y) const;
    void BindMaterial(const SMaterial& material) const;
    void UnBindMaterial(const SMaterial& material) const;

private:
    CEntityGeometryPassShader shader;
    CProjection* projectionMatrix;

    vec4 clipPlane;

	SubscribersMap dynamicSubscribes_;
	std::vector<SubscribersMap> subscribes_;

    const uint32 gridSize     = 1000;
    const uint32 gridCellSize = 250;
};
