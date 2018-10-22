#pragma once
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "Shaders/TreeShader.h"

class CProjection;
struct SMaterial;
class CEntity;
class CModel;
class CMesh;

namespace GameEngine
{
struct RendererContext;
class ModelWrapper;

struct TreeSubscriber
{
    mat4 transform;
    std::vector<vec3>* positions;
    ModelWrapper* top;
    ModelWrapper* bottom;
    uint32 positionTexture;
    vec2ui range;
    vec2 spotCenterPosition;
    bool textureInGpu = false;
};

class TreeRenderer : public IRenderer
{
    typedef std::unordered_map<uint32_t, TreeSubscriber> SubscribersMap;

public:
    TreeRenderer(RendererContext& context);
    // Loading lights itp to shader
    virtual void Init() override;
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void UnSubscribe(GameObject* gameObject) override;
    virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;
    void Render(Scene* scene);

private:
    void PreparePositionMap(TreeSubscriber& sub);
    void RenderModel(CModel* model, const mat4&, uint32) const;
    void RenderMesh(const CMesh& mesh, const mat4&, uint32) const;
    void RenderTrees();
    void BindMaterial(const SMaterial& material) const;
    void UnBindMaterial(const SMaterial& material) const;

private:
    RendererContext& context_;
    TreeShader shader;

    vec4 clipPlane;

    SubscribersMap subscribes_;
    std::list<TreeSubscriber*> subscribersToInit_;
};
}  // GameEngine
