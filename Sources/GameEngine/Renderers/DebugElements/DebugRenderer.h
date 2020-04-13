#pragma once
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "GraphicsApi/IGraphicsApi.h"

namespace common
{
class Transform;
}

namespace GameEngine
{
struct Time;
class Mesh;
class Model;
class Entity;
class Projection;
class ModelWrapper;
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
    DebugObject(GraphicsApi::IGraphicsApi& graphicsApi, Model& model, common::Transform& transform);
    ~DebugObject();

    void CreateBuffer();
    void UpdateBuffer();
    void BindBuffer() const;

    GraphicsApi::IGraphicsApi& graphicsApi_;
    Model& model_;
    common::Transform& transform_;
    PerObjectUpdate buffer;
    GraphicsApi::ID perObjectBufferId;
    bool toUpdate_;
};

typedef std::vector<DebugRendererSubscriber> DebugRendererSubscribers;

class DebugRenderer : public IRenderer
{
public:
    DebugRenderer(GraphicsApi::IGraphicsApi& graphicsApi, Projection& projection);
    ~DebugRenderer();

    virtual void Init() override;
    virtual void ReloadShaders() override;
    void Render(const Scene&, const Time&);
    void SetPhysicsDebugDraw(std::function<const GraphicsApi::LineMesh&()>);
    void AddDebugObject(Model&, common::Transform&);
    void Enable();
    void Disable();

private:
    void DrawPhysics(const mat4&);
    void CreateDebugObjects();
    void UpdateDebugObjectsIfNeeded();
    void DrawGrid();
    void DrawDebugObjects();
    void RenderModel(const Model&) const;
    void BindMeshBuffers(const Mesh&) const;
    void RenderMesh(const Mesh&) const;

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    Projection& projection_;
    ShaderProgram debugObjectShader_;
    ShaderProgram gridShader_;
    ShaderProgram lineShader_;

    std::function<const GraphicsApi::LineMesh&()> physicsDebugDraw_;

    std::vector<DebugObject> debugObjects_;
    std::vector<DebugObject*> toCreateDebugObjects_;
    GraphicsApi::ID gridPerObjectUpdateBufferId_;

    GraphicsApi::ID lineMeshId_;

    bool isActive_;
};

}  // namespace GameEngine
