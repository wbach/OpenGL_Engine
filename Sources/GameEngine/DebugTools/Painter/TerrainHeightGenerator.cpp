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
namespace
{
std::vector<float> noiseSeed;

// clang-format off
std::vector<float> gaussian3x3 = {
    0.077847, 0.123317, 0.077847,
    0.123317, 0.195346, 0.123317,
    0.077847, 0.123317, 0.077847
};
std::vector<float> gaussian5x5 = {
0.003765, 0.015019, 0.023792, 0.015019, 0.003765,
0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
0.023792, 0.094907, 0.150342, 0.094907, 0.023792,
0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
0.003765, 0.015019, 0.023792, 0.015019, 0.003765
};
std::vector<float> gaussian7x7 = {
0.000036, 0.000363, 0.001446, 0.002291, 0.001446, 0.000363, 0.000036,
0.000363, 0.003676, 0.014662, 0.023226, 0.014662, 0.003676, 0.000363,
0.001446, 0.014662, 0.058488, 0.092651, 0.058488, 0.014662, 0.001446,
0.002291, 0.023226, 0.092651, 0.146768, 0.092651, 0.023226, 0.002291,
0.001446, 0.014662, 0.058488, 0.092651, 0.058488, 0.014662, 0.001446,
0.000363, 0.003676, 0.014662, 0.023226, 0.014662, 0.003676, 0.000363,
0.000036, 0.000363, 0.001446, 0.002291, 0.001446, 0.000363, 0.000036
};
// clang-format on
}  // namespace
TerrainHeightGenerator::TerrainHeightGenerator(const Components::ComponentController& componentController,
                                               const EntryParamters& parmaters)
    : componentController_(componentController)
    , perTerrainHeightMapsize_(parmaters.perTerrainHeightMapsize)
    , octaves_(parmaters.octaves)
    , bias_(parmaters.bias)
    , scale_(parmaters.scale)
    , heightFactor_(parmaters.heightFactor)
{
    if (bias_ < 0.1f)
        bias_ = 0.1f;

    uint32 maxOctaves = 0;
    for (uint32 o = 0; o < octaves_; o++)
    {
        uint32 pitch = perTerrainHeightMapsize_.x >> o;
        if (pitch > 0)
        {
            ++maxOctaves;
        }
        else
        {
            break;
        }
    }
    DEBUG_LOG("maxOctaves : " + std::to_string(maxOctaves));
    if (octaves_ > maxOctaves)
    {
        octaves_ = maxOctaves;
        DEBUG_LOG("To hight value of octaves. Set max value");
    }
}

void TerrainHeightGenerator::generateNoiseSeed()
{
    createSeed();
}

void TerrainHeightGenerator::generateHeightMapsImage()
{
    auto count = perTerrainHeightMapsize_.x * perTerrainHeightMapsize_.y;
    if (count != noiseSeed.size())
    {
        DEBUG_LOG("regenerate noise seed");
        createSeed();
    }

    getAllSceneTerrains();

    if (terrains_.size() != 1)
    {
        DEBUG_LOG("Support only 1 terrain, Current terrain size : " + std::to_string(terrains_.size()));
        return;
    }

    perlinNoise2D();
}

void TerrainHeightGenerator::createSeed()
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(0.f, 1.f);

    auto count = perTerrainHeightMapsize_.x * perTerrainHeightMapsize_.y;

    noiseSeed.clear();
    noiseSeed.resize(count);

    for (auto& noise : noiseSeed)
    {
        // noise = dist(mt);
        noise = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }
    DEBUG_LOG("Noise size : " + std::to_string(noiseSeed.size()));
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

float interpolate(float a, float b, float blend, Interpolation interpolation = Interpolation::Linear)
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
    if (noiseSeed.empty())
        return;

    auto width  = perTerrainHeightMapsize_.x;
    auto height = perTerrainHeightMapsize_.y;

    DEBUG_LOG("Start generating terrains.");
    for (auto& terrain : terrains_)
    {
        auto& heightMap = *terrain->GetHeightMap();

        GraphicsApi::Image image;
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
                float scale    = scale_;

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

                    scaleAcc += 1.f;  // scale;
                    noise += (blendY * (sampleB - sampleT) + sampleT) * scale;
                    // noise = interpolate(sampleT, sampleB, blendY) * scale;
                    scale = scale / bias_;
                }

                float normalizedHeight = 2.f * noise / scaleAcc - 1.f;
                image.setPixel(vec2ui(x, y), Color((heightFactor_ * normalizedHeight), 0.f, 0.f, 0.f));
            }
        }
        image.applyFilter(gaussian7x7, vec2ui(7, 7));
        heightMap.setImage(std::move(image));
        terrain->HeightMapChanged();
    }
    DEBUG_LOG("completed");
}

float TerrainHeightGenerator::getNoiseSample(uint32 x, uint32 y)
{
    auto index = x + perTerrainHeightMapsize_.x * y;
    if (index < noiseSeed.size())
        return noiseSeed[index];

    ERROR_LOG("Out of range : " + std::to_string(vec2ui(x, y)) + " (" + std::to_string(index) + "/" +
              std::to_string(noiseSeed.size()) + ")");
    return 0.f;
}
}  // namespace GameEngine
