#pragma once
#include <Types.h>
#include <Utils/Image/Image.h>

#include <vector>

#include "Command.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainTexturesTypes.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"
#include "Logger/Log.h"
#include "TextureCommandData.h"
#include "TextureCommandEntry.h"

class MultipleBlendmapPainterCommand : public Command
{
public:
    MultipleBlendmapPainterCommand(std::vector<TextureCommandEntry>&& input)
    {
        data.reserve(input.size());
        for (auto& entry : input)
        {
            if (auto texture = entry.component.GetTexture(GameEngine::TerrainTextureType::blendMap))
            {
                if (auto blendmap = dynamic_cast<GameEngine::GeneralTexture*>(texture))
                {
                    TextureCommandData item{.component = entry.component,
                                            .snapshot  = std::move(entry.snapshot),
                                            .current   = blendmap->GetImage().getImageData()};
                    data.push_back(std::move(item));
                }
            }
        }
    }

    void redo() override
    {
        for (const auto& entry : data)
        {
            if (auto texture = entry.component.GetTexture(GameEngine::TerrainTextureType::blendMap))
            {
                if (auto blendmap = dynamic_cast<GameEngine::GeneralTexture*>(texture))
                {
                    auto copyImage = entry.current;
                    blendmap->setImageData(std::move(copyImage));
                    entry.component.BlendMapChanged();
                }
            }
        }
    }

    void undo() override
    {
        for (const auto& entry : data)
        {
            if (auto texture = entry.component.GetTexture(GameEngine::TerrainTextureType::blendMap))
            {
                if (auto blendmap = dynamic_cast<GameEngine::GeneralTexture*>(texture))
                {
                    auto copyImage = entry.snapshot;
                    blendmap->setImageData(std::move(copyImage));
                    entry.component.BlendMapChanged();
                }
            }
        }
    }

private:
    std::vector<TextureCommandData> data;
};