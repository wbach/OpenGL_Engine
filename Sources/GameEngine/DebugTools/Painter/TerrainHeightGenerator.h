#pragma once
#include <vector>
#include <Types.h>

namespace GameEngine
{
namespace Components
{
class ComponentController;
class TerrainRendererComponent;
}
class TerrainHeightGenerator
{
public:
    TerrainHeightGenerator(const Components::ComponentController&, const vec2ui&, uint32, float, float);
    void generateHeightMapsImage();

private:
    void createSeed();
    void getAllSceneTerrains();
    void perlinNoise2D();
    float getNoiseSample(uint32 x, uint32 y);

private:
    const Components::ComponentController& componentController_;
    std::vector<Components::TerrainRendererComponent*> terrains_;
    std::vector<float> noiseSeed_;
    vec2ui perTerrainHeightMapsize_;
    uint32 octaves_;
    float bias_;
    float heightFactor_;
};
}  // namespace GameEngine
