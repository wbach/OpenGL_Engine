#pragma once

#include <Common/Transform.h>
#include <GameEngine/Objects/GameObject.h>
#include <GameEngine/Scene/SceneWriter.h>
#include <Logger/Log.h>
#include <Types.h>

#include <GameEngine/Scene/Scene.hpp>

#include "Command.h"
#include "Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "Image/Image.h"
#include "Resources/Textures/HeightMap.h"

class HeightPainterCommand : public Command
{
public:
    HeightPainterCommand(GameEngine::Components::TerrainRendererComponent& component, Utils::ImageData&& snapshot)
        : component_(component)
        , snapshot_{std::move(snapshot)}
    {
        if (auto heightMap = component_.GetHeightMap())
        {
            currentImage_ = heightMap->GetImage().getImageData();
        }
    }

    void redo() override
    {
        if (auto heightMap = component_.GetHeightMap())
        {
            heightMap->setImageData(std::move(currentImage_));
            component_.HeightMapChanged();
        }
    }

    void undo() override
    {
        if (auto heightMap = component_.GetHeightMap())
        {
            heightMap->setImageData(std::move(snapshot_));
            component_.HeightMapChanged();
        }
    }

private:
    GameEngine::Components::TerrainRendererComponent& component_;
    Utils::ImageData snapshot_;
    Utils::ImageData currentImage_;
};