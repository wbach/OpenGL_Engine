#pragma once
#include <Types.h>

#include <vector>

#include "Command.h"
#include "Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "Image/Image.h"
#include "Resources/Textures/HeightMap.h"

class MultipleHeightPainterCommand : public Command
{
private:
    struct Data
    {
        GameEngine::Components::TerrainRendererComponent& component;
        Utils::ImageData snapshot;
        Utils::ImageData current;
    };

public:
    struct Entry
    {
        GameEngine::Components::TerrainRendererComponent& component;
        Utils::ImageData snapshot;
    };

    MultipleHeightPainterCommand(std::vector<Entry>&& input)
    {
        data.reserve(input.size());
        for (auto& entry : input)
        {
            if (auto heightMap = entry.component.GetHeightMap())
            {
                Data item{.component = entry.component,
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
    std::vector<Data> data;
};