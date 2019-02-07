#pragma once
#include "GraphicsApi/IGraphicsApi.h"
#include "GameEngine/Renderers/IRenderer.h"

namespace GameEngine
{
class Mesh;
class Model;
class Entity;
class Projection;
class ModelWrapper;
struct RendererContext;
struct Material;
class IShaderFactory;
class IShaderProgram;

struct EntitySubscriber
{
    uint32 textureIndex;
    GameObject* gameObject;
    ModelWrapper* model;
};

typedef std::unordered_map<uint32_t, EntitySubscriber> SubscribersMap;

class EntityRenderer : public IRenderer
{
public:
    EntityRenderer(RendererContext& context);
    // Loading lights itp to shader
    virtual void Init() override;
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void UnSubscribe(GameObject* gameObject) override;
    virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;
    void Render(Scene* scene);

private:
    void InitShader();
    void PrepareFrame(Scene* scene);
    void RenderModel(Model* model, const mat4& transform_matrix) const;
    void RenderMesh(const Mesh& mesh, const mat4& transform_matrix) const;
    void RenderEntities();
    void BindMaterial(const Material& material) const;
    void UnBindMaterial(const Material& material) const;

private:
    RendererContext& context_;
    std::unique_ptr<IShaderProgram> shader_;

    uint32 currentTextureIndex_ = 0;
    SubscribersMap subscribes_;

    GraphicsApi::ID perFrameId;
    GraphicsApi::ID perObjectId;
};

}  // GameEngine
