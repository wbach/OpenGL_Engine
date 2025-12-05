#pragma once
#include <Types.h>
#include <Utils/Image/Image.h>

#include <vector>

#include "Command.h"
#include "Components/Renderer/Grass/GrassRendererComponent.h"

class PlantPainterCommand : public Command
{
private:
public:
    using GrassRendererComponent = GameEngine::Components::GrassRendererComponent;
    using GrassMeshes            = GrassRendererComponent::GrassMeshes;

    struct Entry
    {
        GrassRendererComponent& component;
        GrassMeshes snapshot;
    };

    struct Data
    {
        GrassRendererComponent& component;
        GrassMeshes snapshot;
        GrassMeshes current;
    };

    using Entries = std::vector<Entry>;
    using Datas   = std::vector<Data>;

    PlantPainterCommand(Entries&& entries)
    {
        for (auto& entry : entries)
        {
            currentDatas_.push_back(Data{.component = entry.component,
                                         .snapshot  = std::move(entry.snapshot),
                                         .current   = entry.component.GetGrassMeshesData()});
        }
    }

    void redo() override
    {
        for (const auto& data : currentDatas_)
        {
            auto copyData = data.current;
            data.component.SetMeshesData(std::move(copyData));
            data.component.UpdateModel();
        }
    }

    void undo() override
    {
        for (const auto& data : currentDatas_)
        {
            auto copyData = data.snapshot;
            data.component.SetMeshesData(std::move(copyData));
            data.component.UpdateModel();
        }
    }

private:
    Datas currentDatas_;
};