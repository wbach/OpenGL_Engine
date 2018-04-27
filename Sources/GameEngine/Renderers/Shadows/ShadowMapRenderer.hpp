#pragma once
#include "ShadowBox.h"
#include "Shaders/ShadowShader.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Renderers/Renderer.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Api/IGraphicsApi.h"

class CMesh;
class CProjection;
class CShadowFrameBuffer;
struct SMaterial;

namespace GameEngine
{
	struct ICamera;
	class ModelWrapper;
}

class CShadowMapRenderer : public CRenderer
{
	struct Subscriber
	{
		uint32 textureIndex = 0;
		CGameObject* gameObject = nullptr;
		GameEngine::ModelWrapper* model = nullptr;
	};
	typedef std::unordered_map<uint32_t, Subscriber> SubscribersMap;

public:
    CShadowMapRenderer(GameEngine::IGraphicsApiPtr graphicsApi, CProjection* projection, GameEngine::RendererContext* rendererContext);
    virtual void Init() override;
    virtual void PrepareFrame(GameEngine::Scene* scene) override;
    virtual void Render(GameEngine::Scene* scene) override;
    virtual void EndFrame(GameEngine::Scene* scene) override;
    virtual void Subscribe(CGameObject* gameObject) override;
	virtual void ReloadShaders() override;

private:
    void PrepareRender(GameEngine::Scene*);
    void PrepareShader(GameEngine::ICamera*) const;
    void RenderSubscribes() const;
    void RenderSubscriber(const Subscriber&) const;
    void RenderMesh(const CMesh& mesh, const mat4& transform_matrix, uint32 textureIndex) const;
    void BindMaterial(const SMaterial&, uint32 textureIndex) const;

private:
	GameEngine::IGraphicsApiPtr graphicsApi_;
    CProjection* projection;
	GameEngine::RendererContext* rendererContext_;
    CShadowShader shader;
    CShadowBox shadowBox;
	CShadowBox shadowBox2;
    mat4 projectionViewMatrix;
	mat4 viewOffset;
	SubscribersMap subscribes_;
};
