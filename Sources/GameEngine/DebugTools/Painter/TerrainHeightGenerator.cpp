#include "TerrainHeightGenerator.h"

#include <GraphicsApi/ImageFilters.h>
#include <Logger/Log.h>

#include <algorithm>
#include <numeric>
#include <random>

#include "GameEngine/Components/ComponentController.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/Textures/HeightMap.h"

namespace GameEngine
{
enum class Interpolation
{
    linear,
    cosinus,
    smoothstep
};
namespace
{
std::vector<float> noiseSeed;
}  // namespace
TerrainHeightGenerator::TerrainHeightGenerator(const Components::ComponentController& componentController,
                                               const EntryParamters& parmaters)
    : componentController_(componentController)
    , gameObjectId_(parmaters.gameObjectId)
    , perTerrainHeightMapsize_(parmaters.perTerrainHeightMapsize)
    , octaves_(parmaters.octaves)
    , bias_(parmaters.bias)
    , scale_(parmaters.scale)
    , heightFactor_(parmaters.heightFactor)
{
    if (bias_ < 0.1f)
        bias_ = 0.1f;

    if (perTerrainHeightMapsize_.x % 2 == 0)
    {
        perTerrainHeightMapsize_.x += 1;
    }

    if (perTerrainHeightMapsize_.y % 2 == 0)
    {
        perTerrainHeightMapsize_.y += 1;
    }

    DEBUG_LOG("perTerrainHeightMapsize_ " + std::to_string(perTerrainHeightMapsize_));

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

    if (not gameObjectId_)
    {
        getAllSceneTerrains();
    }
    else
    {
        getTerrain();
    }

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

void TerrainHeightGenerator::getTerrain()
{
    const auto& components = componentController_.GetAllComonentsOfType<Components::TerrainRendererComponent>();

    for (auto& [_, terrain] : components)
    {
        if (terrain->getParentGameObject().GetId() == *gameObjectId_)
            terrains_.push_back(static_cast<Components::TerrainRendererComponent*>(terrain));
    }
}

void TerrainHeightGenerator::getAllSceneTerrains()
{
    const auto& components = componentController_.GetAllComonentsOfType<Components::TerrainRendererComponent>();

    for (auto& [_, terrain] : components)
    {
        terrains_.push_back(static_cast<Components::TerrainRendererComponent*>(terrain));
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
float cubicHermite(float A, float B, float C, float D, float t)
{
    float a = -A / 2.0f + (3.0f * B) / 2.0f - (3.0f * C) / 2.0f + D / 2.0f;
    float b = A - (5.0f * B) / 2.0f + 2.0f * C - D / 2.0f;
    float c = -A / 2.0f + C / 2.0f;
    float d = B;

    return a * t * t * t + b * t * t + c * t + d;
}

float clamp(float x, float lowerlimit, float upperlimit)
{
    if (x < lowerlimit)
        x = lowerlimit;
    if (x > upperlimit)
        x = upperlimit;
    return x;
}

float smoothstep(float edge0, float edge1, float x)
{
    // Scale, bias and saturate x to 0..1 range
    x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    // Evaluate polynomial
    return x * x * (3 - 2 * x);
}

float interpolate(float a, float b, float blend, Interpolation interpolation = Interpolation::linear)
{
    switch (interpolation)
    {
        case Interpolation::linear:
            return linearInterpolation(a, b, blend);
        case Interpolation::cosinus:
            return cosinusInterpolation(a, b, blend);
        case Interpolation::smoothstep:
            return smoothstep(a, b, blend);
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
        if (not terrain->GetHeightMap())
        {
            //terrain->createHeightMap(perTerrainHeightMapsize_);
            //if (not terrain->GetHeightMap())
            //{
            //    ERROR_LOG("Unsuccessful create heightmap");
            //    continue;
            //}
            continue;
        }

        auto& heightMap = *terrain->GetHeightMap();

        std::vector<float> heights;
        heights.resize(width * height);

        for (uint32 y = 0; y < height; y++)
        {
            for (uint32 x = 0; x < width; x++)
            {
                float noise    = 0.0f;
                float scale    = 1.f;
                float scaleAcc = 0.f;

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

                    noise += (blendY * (sampleB - sampleT) + sampleT) * scale;

                    scaleAcc += scale;
                    scale = scale / bias_;
                }

                heights[x + y * width] = noise / scaleAcc;
            }
        }

        GraphicsApi::Image image;
        image.width  = width;
        image.height = height;
        image.setChannels(1);
        image.moveData(heights);

        image.applyFilter(GraphicsApi::gaussian7x7Filter());
        heightMap.setImage(std::move(image));
        terrain->HeightMapChanged();
    }
    DEBUG_LOG("completed");
}

float TerrainHeightGenerator::getNoiseSample(uint32 x, uint32 y)
{
    uint32 offset = 10;
    if (x < offset or y < offset or x > (perTerrainHeightMapsize_.x - offset) or
        y > (perTerrainHeightMapsize_.y - offset))
        return 0.f;

    auto index = x + perTerrainHeightMapsize_.x * y;
    if (index < noiseSeed.size())
        return noiseSeed[index];

    ERROR_LOG("Out of range : " + std::to_string(vec2ui(x, y)) + " (" + std::to_string(index) + "/" +
              std::to_string(noiseSeed.size()) + ")");
    return 0.f;
}
}  // namespace GameEngine
