#pragma once
#include <GraphicsApi/IGraphicsApi.h>
#include "TerrainMeshRenderer.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class Model;
class Mesh;
class Texture;
class Projection;
class TerrainWrapper;
struct RendererContext;

typedef TerrainWrapper* TerrainPtr;

namespace Components
{
class TerrainMeshRendererComponent;
}  // namespace Components

class ConcreteTerrainMeshRenderer : public IRenderer
{
    struct Subscriber
    {
        GameObject* gameObject_{nullptr};
        Components::TerrainMeshRendererComponent* component_{nullptr};
    };

public:
    ConcreteTerrainMeshRenderer(RendererContext& context);
    ~ConcreteTerrainMeshRenderer();

    void init() override;
    void subscribe(GameObject&) override;
    void unSubscribe(GameObject&) override;
    void reloadShaders() override;
    void render() override;

private:
    void bindShadowMap(uint32 id, uint32 nr) const;

private:
    RendererContext& context_;
    TerrainMeshRenderer terrainMeshRenderer_;
    MeasurementValue* measurementValue_;
    ShaderProgram shader_;
};
}  // namespace GameEngine
