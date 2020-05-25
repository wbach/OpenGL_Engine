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
    TerrainHeightGenerator(const Components::ComponentController&, const vec2ui&);
    void generateHeightMapsImage();

private:
    void createSeed();
    void getAllSceneTerrains();
    void perlinNoise2D();

private:
    const Components::ComponentController& componentController_;
    std::vector<Components::TerrainRendererComponent*> terrains_;
    std::vector<float> noiseSeed_;
    //std::vector<float> perlinNoise_;
    vec2ui perTerrainHeightMapsize_;
    uint32 octaves_;
    float bias_;
};
}  // namespace GameEngine
