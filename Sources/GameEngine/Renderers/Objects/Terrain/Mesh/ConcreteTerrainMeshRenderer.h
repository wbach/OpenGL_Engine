#pragma once
#include <GraphicsApi/IGraphicsApi.h>
#include "TerrainMeshRenderer.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
struct RendererContext;

class ConcreteTerrainMeshRenderer : public IRenderer
{
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
