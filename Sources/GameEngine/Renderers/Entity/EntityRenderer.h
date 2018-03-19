#pragma once
#include "../Renderer.h"
#include "Shaders/GeometryEntityShader.h"
#include "GameEngine/Api/IGraphicsApi.h"

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
    CEntityRenderer(GameEngine::IGraphicsApiPtr graphicsApi, CProjection* projection_matrix, CFrameBuffer* framebuffer);
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
    void RenderEntities();
    void BindMaterial(const SMaterial& material) const;
    void UnBindMaterial(const SMaterial& material) const;

private:
	GameEngine::IGraphicsApiPtr graphicsApi_;
    CEntityGeometryPassShader shader;
    CProjection* projectionMatrix;

    vec4 clipPlane;

	SubscribersMap subscribes_;
};
