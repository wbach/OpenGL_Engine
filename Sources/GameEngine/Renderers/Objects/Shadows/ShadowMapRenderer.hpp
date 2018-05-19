#pragma once
#include "ShadowBox.h"
#include "Shaders/ShadowShader.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Renderers/IRenderer.h"
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

class ShadowMapRenderer : public IRenderer
{
	struct ShadowMapSubscriber
	{
		uint32 textureIndex = 0;
		CGameObject* gameObject = nullptr;
		ModelWrapper* model = nullptr;
	};
	typedef std::unordered_map<uint32_t, ShadowMapSubscriber> SubscribersMap;

public:

    ShadowMapRenderer(RendererContext& context);
	virtual void Init() override;
    virtual void Subscribe(CGameObject* gameObject) override;
	virtual void ReloadShaders() override;
    void Render(Scene* scene);

private:
    void PrepareRender(Scene*);
    void PrepareShader(ICamera*) const;
    void RenderSubscribes() const;
    void RenderSubscriber(const ShadowMapSubscriber&) const;
    void RenderMesh(const CMesh& mesh, const mat4& transform_matrix, uint32 textureIndex) const;
    void BindMaterial(const SMaterial&, uint32 textureIndex) const;

private:
	RendererContext& context_;
    CShadowShader shader_;
    CShadowBox shadowBox_;
	CShadowBox shadowBox2_;
    mat4 projectionViewMatrix_;
	mat4 viewOffset_;
	SubscribersMap subscribes_;
};
} // GameEngine
