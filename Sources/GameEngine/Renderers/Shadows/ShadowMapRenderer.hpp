#pragma once
#include "ShadowBox.h"
#include "Shaders/ShadowShader.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Renderers/Renderer.h"
#include "GameEngine/Renderers/RendererContext.h"

class CMesh;
class CCamera;
class CProjection;
class CShadowFrameBuffer;
struct SMaterial;

namespace GameEngine
{
	class ModelWrapper;
}

class CShadowMapRenderer : public CRenderer
{
	struct Subscriber
	{
		CGameObject* gameObject;
		GameEngine::ModelWrapper* model;
	};
	typedef std::unordered_map<uint32_t, Subscriber> SubscribersMap;

public:
    CShadowMapRenderer(CProjection* projection, GameEngine::RendererContext* rendererContext);
    virtual void Init() override;
    virtual void PrepareFrame(GameEngine::Scene* scene) override;
    virtual void Render(GameEngine::Scene* scene) override;
    virtual void EndFrame(GameEngine::Scene* scene) override;
    virtual void Subscribe(CGameObject* gameObject) override;

private:
    void PrepareRender(GameEngine::Scene*);
    void PrepareShader(CCamera*) const;
    void RenderSubscribes() const;
    void RenderSubscriber(const Subscriber&) const;
    void RenderMesh(const CMesh& mesh, const mat4& transform_matrix) const;
    void BindMaterial(const SMaterial&) const;

private:

    CProjection* projection;
	GameEngine::RendererContext* rendererContext_;
    CShadowShader shader;
    CShadowBox shadowBox;
	CShadowBox shadowBox2;
    mat4 projectionViewMatrix;
	mat4 viewOffset;
	SubscribersMap subscribes_;
};
