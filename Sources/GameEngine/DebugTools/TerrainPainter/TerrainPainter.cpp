#include "TerrainPainter.h"

#include <Input/InputManager.h>
#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "GameEngine/Resources/Textures/MaterialTexture.h"
#include "HeightBrushes/CircleAverageHeightBrush.h"
#include "HeightBrushes/CircleConstantHeightBrush.h"
#include "HeightBrushes/CircleLinearHeightBrush.h"

namespace GameEngine
{
TerrainPainter::TerrainPainter(Input::InputManager& inputManager, const CameraWrapper& camera,
                               const Projection& projection, const vec2ui& windowSize,
                               const Components::ComponentController& componentController)
    : paintType_(PaintType::HeightMap)
    , strength_(0.01f)
    , brushSize_(16)
    , heightBrushType_(HeightBrushType::CircleLinear)
    , stepInterpolation_(StepInterpolation::Linear)
    , inputManager_(inputManager)
    , pointGetter_(camera, projection, windowSize, componentController)
    , lmouseKeyIsPressed_(false)
{
    mouseKeyDownSubscribtion_ =
        inputManager_.SubscribeOnKeyDown(KeyCodes::LMOUSE, [this]() { lmouseKeyIsPressed_ = true; });

    mouseKeyUpSubscribtion_ =
        inputManager_.SubscribeOnKeyUp(KeyCodes::LMOUSE, [this]() { lmouseKeyIsPressed_ = false; });
}

TerrainPainter::~TerrainPainter()
{
    if (mouseKeyDownSubscribtion_)
        inputManager_.UnsubscribeOnKeyDown(KeyCodes::LMOUSE, *mouseKeyDownSubscribtion_);

    if (mouseKeyUpSubscribtion_)
        inputManager_.UnsubscribeOnKeyUp(KeyCodes::LMOUSE, *mouseKeyUpSubscribtion_);
}

void TerrainPainter::Paint(const vec2& mousePosition)
{
    switch (paintType_)
    {
        case PaintType::BlendMap:
            PaintBlendMap(mousePosition);
            break;
        case PaintType::HeightMap:
            PaintHeightMap(mousePosition);
            break;
    }
}

void SetPixel(Image& image, const vec2ui& position, const Color& color)
{
    auto startIndex = 4 * (position.x + position.y * image.width);
    if (startIndex + 2 < image.data.size())
    {
        image.data[startIndex]     = color.r();
        image.data[startIndex + 1] = color.g();
        image.data[startIndex + 2] = color.b();
        image.data[startIndex + 3] = color.a();
    }
}

void TerrainPainter::PaintBlendMap(const vec2& mousePosition)
{
    if (not lmouseKeyIsPressed_)
        return;

    auto terrainPoint = pointGetter_.GetMousePointOnTerrain(mousePosition);

    if (not terrainPoint)
        return;

    auto blendMapTexture = terrainPoint->terrainComponent.GetTexture(TerrainTextureType::blendMap);
    if (blendMapTexture)
    {
        auto blendMap       = static_cast<MaterialTexture*>(blendMapTexture);
        auto& blendMapImage = blendMap->GetImage();

        auto& config = terrainPoint->terrainComponent.GetTerrainConfiguration();
        auto posX    = static_cast<float>(terrainPoint->terrainSpacePoint.x) / config.GetScale().x;
        auto posY    = static_cast<float>(terrainPoint->terrainSpacePoint.y) / config.GetScale().z;

        auto imageCoordX = static_cast<int32>(posX * static_cast<float>(blendMapImage.width));
        auto imageCoordY = static_cast<int32>(posY * static_cast<float>(blendMapImage.height));

        for (int32 y = -brushSize_; y < brushSize_; y++)
        {
            for (int32 x = -brushSize_; x < brushSize_; x++)
            {
                if (((x) * (x) + (y) * (y)) <= brushSize_ * brushSize_)
                {
                    vec2ui imageCoord(static_cast<uint32>(imageCoordX + x), static_cast<uint32>(imageCoordY + y));
                    SetPixel(blendMapImage, imageCoord, Color(255, 0, 0));
                }
            }
        }
        terrainPoint->terrainComponent.BlendMapChanged();
    }
}

#define PAINT(X) \
    X(*terrainPoint, stepInterpolation_ == StepInterpolation::Linear, mousePosition, strength_, brushSize_).Paint()

void TerrainPainter::PaintHeightMap(const vec2& mousePosition)
{
    if (not lmouseKeyIsPressed_)
        return;

    auto terrainPoint = pointGetter_.GetMousePointOnTerrain(mousePosition);

    if (not terrainPoint)
        return;

    auto heightMap = terrainPoint->terrainComponent.GetHeightMap();
    if (not heightMap)
    {
        return;
    }

    bool heightmapChange{false};

    switch (heightBrushType_)
    {
        case HeightBrushType::CircleLinear:
            heightmapChange = PAINT(CircleLinearHeightBrush);
            break;
        case HeightBrushType::CircleAverage:
            heightmapChange = PAINT(CircleAverageHeightBrush);
            break;
        case HeightBrushType::CircleConstantValue:
            heightmapChange = PAINT(CircleConstantHeightBrush);
            break;
    }

    if (heightmapChange)
        terrainPoint->terrainComponent.HeightMapChanged();
}
#undef PAINT

std::optional<vec3> TerrainPainter::GetMouseTerrainPosition(const vec2& mousePosition)
{
    auto terrainPoint = pointGetter_.GetMousePointOnTerrain(mousePosition);

    if (terrainPoint)
        return terrainPoint->pointOnTerrain;

    return std::nullopt;
}

void TerrainPainter::RecalculateNormals()
{
    for (auto& terrain : pointGetter_.GetSceneTerrains())
    {
        terrain->RecalculateNormals();
    }
}

void TerrainPainter::RecalcualteYOffset()
{
    for (auto& terrain : pointGetter_.GetSceneTerrains())
    {
        terrain->RecalculateYOffset();
    }
}
}  // namespace GameEngine
