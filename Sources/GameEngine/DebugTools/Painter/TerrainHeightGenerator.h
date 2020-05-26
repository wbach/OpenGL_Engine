#pragma once
#include <Types.h>

#include <vector>

namespace GameEngine
{
namespace Components
{
class ComponentController;
class TerrainRendererComponent;
}  // namespace Components

class TerrainHeightGenerator
{
public:
    struct EntryParamters
    {
        float bias{2.f};
        float scale{10.f};
        uint32 octaves{9};
        float heightFactor{20.f};
        vec2ui perTerrainHeightMapsize{512, 512};
    };

    TerrainHeightGenerator(const Components::ComponentController&, const EntryParamters&);
    void generateNoiseSeed();
    void generateHeightMapsImage();

private:
    void createSeed();
    void getAllSceneTerrains();
    void perlinNoise2D();
    float getNoiseSample(uint32 x, uint32 y);

private:
    const Components::ComponentController& componentController_;
    std::vector<Components::TerrainRendererComponent*> terrains_;
    vec2ui perTerrainHeightMapsize_;
    uint32 octaves_;
    float bias_;
    float scale_;
    float heightFactor_;
};
}  // namespace GameEngine
