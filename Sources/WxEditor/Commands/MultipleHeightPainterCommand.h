#pragma once
#include <Types.h>
#include <Utils/Image/Image.h>

#include <vector>

#include "Command.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "TextureCommandData.h"
#include "TextureCommandEntry.h"

class MultipleHeightPainterCommand : public Command
{
public:
    MultipleHeightPainterCommand(std::vector<TextureCommandEntry>&& input)
    {
        data.reserve(input.size());
        for (auto& entry : input)
        {
            if (auto heightMap = entry.component.GetHeightMap())
            {
                TextureCommandData item{.component = entry.component,
                                        .snapshot  = std::move(entry.snapshot),
                                        .current   = heightMap->GetImage().getImageData()};
                data.push_back(std::move(item));
            }
        }
    }

    void redo() override
    {
        for (const auto& entry : data)
        {
            if (auto heightMap = entry.component.GetHeightMap())
            {
                auto copyImage = entry.current;
                heightMap->setImageData(std::move(copyImage));
                entry.component.HeightMapChanged();
            }
        }
    }

    void undo() override
    {
        for (const auto& entry : data)
        {
            if (auto heightMap = entry.component.GetHeightMap())
            {
                auto copyImage = entry.snapshot;
                heightMap->setImageData(std::move(copyImage));
                entry.component.HeightMapChanged();
            }
        }
    }

private:
    std::vector<TextureCommandData> data;
};