#include "FlattenHeightPainter.h"

#include <Input/InputManager.h>
#include <Input/KeyCodes.h>
#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/DebugTools/Painter/HeightPainter.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "IBrush.h"

namespace GameEngine
{

FlattenHeightPainter::FlattenHeightPainter(TerrainPainter::Dependencies&& dependencies, std::unique_ptr<IBrush> brush)
    : HeightPainter(std::move(dependencies), std::move(brush))
{
    keySubId = inputManager_.SubscribeOnKeyDown(KeyCodes::LMOUSE,
                                                [&]()
                                                {
                                                    auto currentTerrainPoint =
                                                        pointGetter_.GetMousePointOnTerrain(inputManager_.GetMousePosition());
                                                    if (currentTerrainPoint)
                                                    {
                                                        auto heightMap = currentTerrainPoint->terrainComponent->GetHeightMap();
                                                        if (heightMap)
                                                        {
                                                            vec2ui point = ConvertToPixelSpace(*currentTerrainPoint);
                                                            firstHeight  = heightMap->GetHeight(point);
                                                        }
                                                    }
                                                });

    //  keySubId = inputManager_.SubscribeOnKeyUp(KeyCodes::LMOUSE,[&](){ firstHeight.reset()});
}

FlattenHeightPainter::~FlattenHeightPainter()
{
    inputManager_.UnsubscribeOnKeyDown(KeyCodes::LMOUSE, keySubId);
}
void FlattenHeightPainter::Apply(Texture& texture, const vec2ui& paintedPoint, const Influance& influancePoint,
                                 DeltaTime deltaTime)
{
    try
    {
        auto& heightmap = dynamic_cast<HeightMap&>(texture);
        if (not IsInRange(heightmap.GetImage(), paintedPoint))
        {
            LOG_WARN << "Out of range";
            return;
        }

        if (firstHeight)
            heightmap.SetHeight(paintedPoint, *firstHeight);
    }
    catch (const std::bad_cast& e)
    {
        std::cerr << "Bad cast: " << e.what() << std::endl;
    }
}
}  // namespace GameEngine
