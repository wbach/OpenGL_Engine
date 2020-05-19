#pragma once
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "PhysicsVisualizator.h"
#include <Mutex.hpp>

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
    DebugObject(GraphicsApi::IGraphicsApi&, Model&, common::Transform&);
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
    DebugRenderer(GraphicsApi::IGraphicsApi&, Utils::Thread::ThreadSync&);
    ~DebugRenderer();

    void Init() override;
    void ReloadShaders() override;

    void Render(const Scene&, const Time&);
    void SetPhysicsDebugDraw(std::function<const GraphicsApi::LineMesh&()>);
    void AddDebugObject(Model&, common::Transform&);
    void Enable();
    void Disable();
    bool IsEnable() const;

    void EnablePhysics();
    void DisablPhysics();
    bool IsEnablePhysics() const;
    void ClearDebugObjects();

private:
    void CreateDebugObjects();
    void UpdateDebugObjectsIfNeeded();
    void RenderDebugObjects();
    void LoadDefaultPerObjectBuffer();
    void DrawGrid();
    void DrawDebugObjects();
    void RenderModel(const Model&) const;
    void BindMeshBuffers(const Mesh&) const;

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    PhysicsVisualizator physicsVisualizator_;

    ShaderProgram debugObjectShader_;
    ShaderProgram gridShader_;
    ShaderProgram lineShader_;

    std::vector<DebugObject> debugObjects_;
    std::vector<DebugObject*> toCreateDebugObjects_;
    GraphicsApi::ID gridPerObjectUpdateBufferId_;
    bool isActive_;
    std::mutex debugObjectsMutex_;
};

}  // namespace GameEngine
