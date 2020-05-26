#include "TerrainHeightGenerator.h"

#include <Logger/Log.h>

#include <algorithm>
#include <numeric>
#include <random>

#include "GameEngine/Components/ComponentController.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
TerrainHeightGenerator::TerrainHeightGenerator(const Components::ComponentController& componentController,
                                               const vec2ui& perTerrainHeightMapsize, uint32 octaves, float bias,
                                               float heightFactor)
    : componentController_(componentController)
    , perTerrainHeightMapsize_(perTerrainHeightMapsize)
    , octaves_(octaves)
    , bias_(bias)
    , heightFactor_(heightFactor)
{
    if (bias < 0.1f)
        bias = 0.1f;
    if (octaves_ > 10)
        octaves = 10;
}

void TerrainHeightGenerator::generateHeightMapsImage()
{
    getAllSceneTerrains();
    createSeed();
    perlinNoise2D();
}

void TerrainHeightGenerator::createSeed()
{
    if (terrains_.size() != 1)
        return;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(0.f, 1.f);

    auto count = perTerrainHeightMapsize_.x * perTerrainHeightMapsize_.y;
    noiseSeed_.resize(count);

    for (auto& noise : noiseSeed_)
    {
        // noise = dist(mt);
        noise = (float)rand() / (float)RAND_MAX;
    }
}

void TerrainHeightGenerator::getAllSceneTerrains()
{
    const auto& components = componentController_.GetAllComonentsOfType(Components::ComponentsType::TerrainRenderer);

    for (auto& terrain : components)
    {
        terrains_.push_back(static_cast<Components::TerrainRendererComponent*>(terrain.second));
    }
}

float linearInterpolation(float a, float b, float blend)
{
    return (1.0f - blend) * a + blend * b;
}
float cosinusInterpolation(float a, float b, float blend)
{
    float theta = blend * M_PI;
    float f     = (1.f - cosf(theta)) * .5f;
    return a * (1.f - f) + b * f;
}

void TerrainHeightGenerator::perlinNoise2D()
{
    auto width  = perTerrainHeightMapsize_.x;
    auto height = perTerrainHeightMapsize_.y;

    for (auto& terrain : terrains_)
    {
        auto heightMap = terrain->GetHeightMap();
        auto& image    = heightMap->GetImage();

        if (terrain->GetMeshTerrain())
        {
            if (not compare(width, image.width) or not compare(height, image.height))
            {
                width  = image.width;
                height = image.height;

                DEBUG_LOG(
                    "Currently not supported resize current heightmap for terrain mesh renderer. (To do : recreacte "
                    "whole "
                    "mesh). Size set to : " +
                    std::to_string(image.size()));
            }
        }

        image        = GraphicsApi::Image();
        image.width  = width;
        image.height = height;
        image.setChannels(1);
        image.allocateImage<float>();

        for (uint32 x = 0; x < width; x++)
        {
            for (uint32 y = 0; y < height; y++)
            {
                float noise    = 0.0f;
                float scaleAcc = 0.0f;
                float scale    = 1.f;

                for (uint32 o = 0; o < octaves_; o++)
                {
                    uint32 pitch    = width >> o;
                    uint32 sampleX1 = (x / pitch) * pitch;
                    uint32 sampleY1 = (y / pitch) * pitch;

                    uint32 sampleX2 = (sampleX1 + pitch) % width;
                    uint32 sampleY2 = (sampleY1 + pitch) % width;

                    float blendX = static_cast<float>(x - sampleX1) / static_cast<float>(pitch);
                    float blendY = static_cast<float>(y - sampleY1) / static_cast<float>(pitch);

                    // linearInterpolation
                    auto sampleT = cosinusInterpolation(noiseSeed_[sampleY1 * width + sampleX1],
                                                        noiseSeed_[sampleY1 * width + sampleX2], blendX);
                    auto sampleB = cosinusInterpolation(noiseSeed_[sampleY2 * width + sampleX1],
                                                        noiseSeed_[sampleY2 * width + sampleX2], blendX);

                    scaleAcc += scale;
                    noise += (blendY * (sampleB - sampleT) + sampleT) * scale;
                    scale = scale / bias_;
                }

                float normalizedHeight = 2.f * noise / scaleAcc - 1.f;
                image.setPixel(vec2ui(y, x), Color((heightFactor_ * normalizedHeight), 0.f, 0.f, 0.f));
            }
        }
        terrain->HeightMapChanged();
    }
}
}  // namespace GameEngine
