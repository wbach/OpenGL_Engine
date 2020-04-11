#pragma once
#include "GameEngine/Renderers/IRenderer.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "Resources/ResourceManager.h"

namespace GameEngine
{
struct Time;
class Mesh;
class Model;
class Entity;
class Projection;
class ModelWrapper;
struct RendererContext;
struct Material;

namespace Components
{
class RendererComponent;
}  // namespace Components

struct DebugRendererSubscriber
{
    GameObject* gameObject;
    Components::RendererComponent* renderComponent;
};

struct DebugObject
{
    BufferObject<PerObjectUpdate> buffer;
    Model* model{nullptr};
};

typedef std::vector<DebugRendererSubscriber> DebugRendererSubscribers;

class DebugRenderer : public IRenderer
{
public:
    DebugRenderer(const RendererContext& context);
    ~DebugRenderer();

    virtual void Init() override;
    virtual void ReloadShaders() override;
    void Render(const Scene&, const Time&);
    void SetPhysicsDebugDraw(std::function<void(const mat4&, const mat4&)>);

private:
    void DrawGrid();
    void DrawDebugObjects();
    void RenderModel(const Model&) const;
    void BindMeshBuffers(const Mesh&) const;
    void RenderMesh(const Mesh&) const;

private:
    const RendererContext& context_;
    ResourceManager resourceManager_;
    ShaderProgram debugObjectShader_;
    ShaderProgram gridShader_;

    std::function<void(const mat4&, const mat4&)> physicsDebugDraw_;

    std::vector<DebugObject> debugObjects_;

    GraphicsApi::ID gridPerObjectUpdateBufferId_;

//    GraphicsApi::ID perObjectUpdateId;
//    GraphicsApi::ID perObjectId;
};

}  // namespace GameEngine
