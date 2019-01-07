#pragma once
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Renderers/IRenderer.h"

namespace GameEngine
{
class Model;
class Texture;
class Projection;
class TerrainWrapper;
struct RendererContext;
class IShaderProgram;

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
    void BindTexture(Texture* texture, int id) const;
    void BindTextures(TerrainPtr terrain) const;
    void RenderSubscribers(const mat4& viewMatrix, int range) const;
    void RenderSubscriber(TerrainPtr) const;
    void InitShader();

private:
    RendererContext& context_;
    std::unique_ptr<IShaderProgram> shader_;
    vec4 clipPlane;

    TerrainPtrs subscribes;

    uint32 objectId;
};
}
