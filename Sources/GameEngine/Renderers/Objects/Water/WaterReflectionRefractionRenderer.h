#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/Objects/Entity/EntityRenderer.h"
#include "GameEngine/Renderers/Objects/SkyBox/SkyBoxRenderer.h"
#include "GameEngine/Renderers/Objects/Terrain/Mesh/TerrainMeshRenderer.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/ShaderBuffers/ShadowsBuffer.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Shaders/ShaderProgram.h"
#include <set>

namespace GameEngine
{
class Mesh;
class CameraWrapper;
struct Material;
class Projection;
class ShadowFrameBuffer;
class ModelWrapper;
struct Time;

namespace Components
{
class Animator;
class RendererComponent;
}  // namespace Components

class WaterReflectionRefractionRenderer : public IRenderer
{
public:
    struct WaterTextures
    {
        GraphicsApi::ID waterReflectionTextureId;
        GraphicsApi::ID waterRefractionTextureId;
        GraphicsApi::ID waterRefractionDepthTextureId;
    };

    struct WaterFbo
    {
        float positionY;
        GraphicsApi::IFrameBuffer* reflectionFrameBuffer_;
        GraphicsApi::IFrameBuffer* refractionFrameBuffer_;
        WaterTextures waterTextures_;
        std::set<uint32> usingByObjects;
    };

    struct Subscriber
    {
        GameObject& gameObject;
        WaterTextures* waterTextures_{nullptr};
        std::optional<float> waterFboPositionY;
    };

    WaterReflectionRefractionRenderer(RendererContext&);
    ~WaterReflectionRefractionRenderer();

    void init() override;
    void prepare() override;
    void subscribe(GameObject&) override;
    void unSubscribe(GameObject&) override;
    void unSubscribeAll() override;
    void reloadShaders() override;

    WaterReflectionRefractionRenderer::WaterTextures* GetWaterTextures(uint32) const;

private:
    GraphicsApi::IFrameBuffer* createWaterFbo(const vec2ui&);
    void initResources();
    void cleanUp();
    void renderScene();
    void createRefractionTexture(WaterFbo&);
    void createReflectionTexture(WaterFbo&);
    void cleanNotUsedFbos();
    WaterFbo* getFbo(uint32, Subscriber&);
    WaterFbo* findFbo(float);
    WaterFbo* createWaterTilesTextures(float);

private:
    RendererContext& context_;
    EntityRenderer entityRenderer_;
    TerrainMeshRenderer terrainMeshRenderer_;
    SkyBoxRenderer skyBoxRenderer_;

    ShaderProgram entityShader_;
    ShaderProgram terrainShader_;
    ShaderProgram skyBoxShader_;

    GraphicsApi::ID reflectionPerFrameBuffer_;
    GraphicsApi::ID refractionPerFrameBuffer_;

    std::list<WaterFbo> waterFbos_;
    std::unordered_map<uint32, Subscriber> subscribers_;

    std::mutex subscriberMutex_;
    IdType enabledSubscriptionId_;

    bool isInit_;
    bool isActive_;
};
}  // namespace GameEngine