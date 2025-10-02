#pragma once
#include <Mutex.hpp>

#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Resources/BufferObject.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "PhysicsVisualizator.h"

namespace GraphicsApi
{
class IGraphicsApi;
}

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
struct RendererContext;

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

struct DebugMeshInfo
{
    GameObject& gameObject;
    ModelWrapper& modelWrapper;
};

typedef std::vector<DebugRendererSubscriber> DebugRendererSubscribers;

class DebugRenderer : public IRenderer
{
public:
    enum class RenderState
    {
        Physics,
        Grid,
        Objects,
        Normals
    };

    DebugRenderer(RendererContext&, Utils::Thread::ThreadSync&);
    ~DebugRenderer();

    void init() override;
    void reloadShaders() override;
    void render() override;
    void clear();

    void subscribe(GameObject&) override;
    void unSubscribe(GameObject&) override;
    void unSubscribeAll() override;
    void renderTextures(const std::vector<GraphicsApi::ID>&);
    void SetPhysicsDebugDraw(std::function<const GraphicsApi::LineMesh&()>);
    void AddDebugObject(Model&, common::Transform&);
    void Enable();
    void Disable();
    bool IsEnable() const;
    void AddState(RenderState);
    void RemoveState(RenderState);
    bool IsStateEnabled(RenderState) const;
    const std::vector<RenderState>& GetStates() const;
    void clearDebugObjects();

private:
    void CreateDebugObjects();
    void UpdateDebugObjectsIfNeeded();
    void RenderDebugObjects();
    void LoadDefaultPerObjectBuffer();
    void DrawGrid();
    void DrawDebugObjects();
    void DrawNormals();
    void RenderModel(const Model&) const;
    void BindMeshBuffers(const Mesh&) const;

private:
    RendererContext& rendererContext_;
    PhysicsVisualizator physicsVisualizator_;

    ShaderProgram debugObjectShader_;
    ShaderProgram gridShader_;
    ShaderProgram debugNormalShader_;
    ShaderProgram textureShader_;

    std::list<DebugObject> debugObjects_;
    std::list<DebugObject*> toCreateDebugObjects_;
    std::unordered_map<uint32, DebugMeshInfo> meshDebugInfoSubscribers_;

    GraphicsApi::ID meshDebugPerObjectBufferId_;
    GraphicsApi::ID gridPerObjectUpdateBufferId_;
    GraphicsApi::ID texturePerObjectUpdateBufferId_;
    GraphicsApi::ID textureColorBufferId_;
    std::optional<IdType> showPhycicsVisualizationSubId;

    std::mutex meshInfoDebugObjectsMutex_;
    std::mutex debugObjectsMutex_;

    std::vector<RenderState> states_;
    std::vector<RenderState> stashedStates_;
};

}  // namespace GameEngine
