#pragma once
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "Shaders/TerrainShader.h"

class CModel;
class CTexture;
class CProjection;

namespace GameEngine
{
class TerrainWrapper;
struct RendererContext;
typedef TerrainWrapper* TerrainPtr;
typedef std::vector<TerrainPtr> TerrainPtrs;

class TerrainRenderer : public IRenderer
{
public:
    TerrainRenderer(RendererContext& context);
    // Loading lights itp to shader
    virtual void Init() override;
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void UnSubscribe(GameObject* gameObject) override;
    virtual void ReloadShaders() override;
    void Render(Scene* scene);

private:
    void BindTexture(CTexture* texture, int id) const;
    void BindTextures(TerrainPtr terrain) const;
    void RenderSubscribers(const mat4& viewMatrix, int range) const;
    void RenderSubscriber(TerrainPtr) const;
    void InitShader();

private:
    RendererContext& context_;
    TerrainShader shader;
    vec4 clipPlane;

    TerrainPtrs subscribes;

    uint32 objectId;
};
}
