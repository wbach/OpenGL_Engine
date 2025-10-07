#include "TerrainHeightGenerator.h"

#include <Logger/Log.h>
#include <Types.h>
#include <Utils/Image/ImageFilters.h>
#include <Utils/Utils.h>

#include <algorithm>
#include <numeric>
#include <random>
#include <unordered_map>

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
    smoothstep,
    smootherstep
};
namespace
{
std::unordered_map<uint32, float> noiseSeed;
const bool UseZeroBound{false};
}  // namespace
TerrainHeightGenerator::TerrainHeightGenerator(const Components::ComponentController& componentController,
                                               const EntryParamters& parmaters)
    : componentController_(componentController)
    , gameObjectId_(parmaters.gameObjectId)
    , perTerrainHeightMapsize_(parmaters.perTerrainHeightMapsize)
    , octaves_(parmaters.octaves)
    , bias_(parmaters.bias)
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

    if (octaves_ > maxOctaves)
    {
        octaves_ = maxOctaves;
        LOG_DEBUG << "To hight value of octaves. Set max value";
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
        LOG_ERROR << "Support only 1 terrain, Current terrain size : " << terrains_.size();
        return;
    }

    perlinNoise2D();
}

void createTerrainTransition(GameObject& go1, GameObject& go2, float transitionSize)
{
    auto transform1 = go1.GetWorldTransform();
    auto transform2 = go2.GetWorldTransform();

    Components::TerrainRendererComponent* component1{nullptr};
    Components::TerrainRendererComponent* component2{nullptr};

    if (compare(transform1.GetPosition().x, transform2.GetPosition().x))
    {
        if (compare(transform1.GetPosition().z, transform2.GetPosition().z))
        {
            LOG_ERROR << "Both terrain have the same position!";
            return;
        }

        if (transform1.GetPosition().z > transform2.GetPosition().z)
        {
            component1 = go1.GetComponent<Components::TerrainRendererComponent>();
            component2 = go2.GetComponent<Components::TerrainRendererComponent>();
        }
        else
        {
            component1 = go2.GetComponent<Components::TerrainRendererComponent>();
            component2 = go1.GetComponent<Components::TerrainRendererComponent>();
        }

        if (not component1 or not component2)
            return;

        auto heightMap1 = component1->GetHeightMap();
        auto heightMap2 = component2->GetHeightMap();

        if (not heightMap1 or not heightMap2)
        {
            LOG_ERROR << "Heihtmap not set!, heightMap1{" << Utils::BoolToString(heightMap1) << "} heightmap2{"
                      << Utils::BoolToString(heightMap2) << "}";
            return;
        }

        if (heightMap1->GetImage().size() != heightMap2->GetImage().size())
        {
            LOG_ERROR << "Height map are diffrentSize, unsupported";
            return;
        }

        auto scale1 = component1->getParentGameObject().GetWorldTransform().GetScale().y;
        auto scale2 = component2->getParentGameObject().GetWorldTransform().GetScale().y;

        auto size = heightMap1->GetImage().size();
        LOG_DEBUG << "creating transition...";
        for (uint32 x = 0; x < size.x; ++x)
        {
            for (uint32 y = 0; y < transitionSize; ++y)
            {
                vec2ui sourcePixel{x, size.y - y};
                vec2ui targetPixel{x, y};

                auto heightMapValue1 = heightMap1->GetImage().getPixel(targetPixel);
                auto heightMapValue2 = heightMap2->GetImage().getPixel(sourcePixel);

                if (heightMapValue1 and heightMapValue2)
                {
                    // auto targetHeight = heightMapValue1->value.x * scale1;
                    // auto sourceHeight = heightMapValue2->value.x * scale2;
                    // heightMapValue2->value.x * scale2 = heightMapValue1->value.x * scale1;
                    // heightMapValue1->value.x = heightMapValue2->value.x * scale2 / scale1

                    auto newHeight = heightMapValue2->value.x * scale2 / scale1;
                    // auto newHeight = heightMapValue2->value.x * scale2 / scale1 / scale1;
                    LOG_DEBUG << "Old height " << heightMapValue1->value << " | New height " << newHeight;
                    heightMap1->SetHeight(targetPixel, newHeight);
                }
            }
        }
        component1->HeightMapChanged();
    }
    else if (compare(transform1.GetPosition().z, transform2.GetPosition().z))
    {
        if (compare(transform1.GetPosition().x, transform2.GetPosition().x))
        {
            LOG_ERROR << "Both terrain have the same position!";
            return;
        }
    }
    LOG_DEBUG << "Done.";
}

void TerrainHeightGenerator::createSeed()
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(0.f, 1.f);

    auto count = perTerrainHeightMapsize_.x * perTerrainHeightMapsize_.y;

    noiseSeed.clear();

    for (uint32 i = 0; i < count; i++)
    {
        noiseSeed[i] = getRandomFloat(0.f, 1.f);
    }
}

void TerrainHeightGenerator::getTerrain()
{
    const auto& components = componentController_.GetAllComponentsOfType<Components::TerrainRendererComponent>();

    for (auto& terrain : components)
    {
        if (terrain->getParentGameObject().GetId() == *gameObjectId_)
            terrains_.push_back(terrain);
    }
}

void TerrainHeightGenerator::getAllSceneTerrains()
{
    terrains_ = componentController_.GetAllComponentsOfType<Components::TerrainRendererComponent>();
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
float smoothstep(float x)
{
    float edge0 = 0.f;
    float edge1 = 1.f;

    // Scale, bias and saturate x to 0..1 range
    x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    // Evaluate polynomial
    return x * x * (3 - 2 * x);
}
float smootherstep(float x)
{
    float edge0 = 0.f;
    float edge1 = 1.f;

    // Scale, and clamp x to 0..1 range
    x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    // Evaluate polynomial
    return x * x * x * (x * (x * 6 - 15) + 10);
}

float fade(float t)
{
    return ((6 * t - 15) * t + 10) * t * t * t;
}

float interpolate(float a, float b, float blend, Interpolation interpolation = Interpolation::smootherstep)
{
    switch (interpolation)
    {
        case Interpolation::linear:
            return linearInterpolation(a, b, blend);
        case Interpolation::cosinus:
            return cosinusInterpolation(a, b, blend);
        case Interpolation::smoothstep:
            return linearInterpolation(a, b, smoothstep(blend));
        case Interpolation::smootherstep:
            return linearInterpolation(a, b, smootherstep(blend));
    }
    return 0.f;
}

void TerrainHeightGenerator::perlinNoise2D()
{
    if (noiseSeed.empty())
    {
        createSeed();
    }

    auto width  = perTerrainHeightMapsize_.x;
    auto height = perTerrainHeightMapsize_.y;

    LOG_DEBUG << "Start generating terrains.";
    for (auto& terrain : terrains_)
    {
        if (not terrain->GetHeightMap())
        {
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

                    auto sampleT = interpolate(getNoiseSample(sampleX1, sampleY1), getNoiseSample(sampleX2, sampleY1), blendX);
                    auto sampleB = interpolate(getNoiseSample(sampleX1, sampleY2), getNoiseSample(sampleX2, sampleY2), blendX);

                    noise += interpolate(sampleT, sampleB, blendY) * scale;

                    scaleAcc += scale;
                    scale = scale / bias_;
                }

                heights[x + y * width] = noise / scaleAcc;
            }
        }

        Utils::Image image;
        image.width  = width;
        image.height = height;
        image.setChannels(1);
        image.moveData(heights);

        // image.applyFilter(GraphicsApi::gaussian7x7Filter());
        heightMap.setImage(std::move(image));
        terrain->HeightMapChanged();
    }
    LOG_DEBUG << "completed";
}

float TerrainHeightGenerator::getNoiseSample(uint32 x, uint32 y)
{
    auto index = x + perTerrainHeightMapsize_.x * y;

    if (not UseZeroBound)
    {
        auto iter = noiseSeed.find(index);
        if (iter != noiseSeed.end())
        {
            return iter->second;
        }

        auto result = getRandomFloat();
        noiseSeed.insert({index, result});
        return result;
    }

    uint32 offset = 1;
    if (x < offset or y < offset or x > (perTerrainHeightMapsize_.x - 1 - offset) or
        y > (perTerrainHeightMapsize_.y - 1 - offset))
        return 0.0f;

    if (index < noiseSeed.size())
        return noiseSeed[index];

    LOG_ERROR << "Out of range : " << vec2ui(x, y) << " (" << index << "/" << noiseSeed.size() << ")";
    return 0.f;
}
}  // namespace GameEngine
