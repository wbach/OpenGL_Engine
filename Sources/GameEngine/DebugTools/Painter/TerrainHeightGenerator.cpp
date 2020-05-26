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
enum class Interpolation
{
    Linear,
    Cosinus
};
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

    if (terrains_.size() != 1)
    {
        DEBUG_LOG("Support only 1 terrain, Current terrain size : " + std::to_string(terrains_.size()));
        return;
    }

    createSeed();
    perlinNoise2D();
}

void TerrainHeightGenerator::createSeed()
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(0.f, 1.f);

    auto count = perTerrainHeightMapsize_.x * perTerrainHeightMapsize_.y;
    noiseSeed_.resize(count);

    for (auto& noise : noiseSeed_)
    {
        // noise = dist(mt);
        noise = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }
    DEBUG_LOG("Noise size : " + std::to_string(noiseSeed_.size()));
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
    float theta = blend * static_cast<float>(M_PI);
    float f     = (1.f - cosf(theta)) * .5f;
    return a * (1.f - f) + b * f;
}

float interpolate(float a, float b, float blend, Interpolation interpolation = Interpolation::Cosinus)
{
    switch (interpolation)
    {
        case Interpolation::Linear:
            return linearInterpolation(a, b, blend);
        case Interpolation::Cosinus:
            return cosinusInterpolation(a, b, blend);
    }

    return 0.f;
}

void TerrainHeightGenerator::perlinNoise2D()
{
    if (noiseSeed_.empty())
        return;

    auto width  = perTerrainHeightMapsize_.x;
    auto height = perTerrainHeightMapsize_.y;

    DEBUG_LOG("Start generating terrains.");
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

        for (uint32 y = 0; y < height; y++)
        {
            for (uint32 x = 0; x < width; x++)
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

                    auto sampleT =
                        interpolate(getNoiseSample(sampleX1, sampleY1), getNoiseSample(sampleX2, sampleY1), blendX);
                    auto sampleB =
                        interpolate(getNoiseSample(sampleX1, sampleY2), getNoiseSample(sampleX2, sampleY2), blendX);

                    scaleAcc += scale;
                    noise += (blendY * (sampleB - sampleT) + sampleT) * scale;
                    scale = scale / bias_;
                }

                float normalizedHeight = 2.f * noise / scaleAcc - 1.f;
                image.setPixel(vec2ui(x, y), Color((heightFactor_ * normalizedHeight), 0.f, 0.f, 0.f));
            }
        }
        terrain->HeightMapChanged();
    }
    DEBUG_LOG("completed");
}

float TerrainHeightGenerator::getNoiseSample(uint32 x, uint32 y)
{
    auto index = x + perTerrainHeightMapsize_.x * y;
    if (index < noiseSeed_.size())
        return noiseSeed_[index];

    ERROR_LOG("Out of range : " + std::to_string(vec2ui(x, y)) + " (" + std::to_string(index) + "/" +
              std::to_string(noiseSeed_.size()) + ")");
    return 0.f;
}
}  // namespace GameEngine
