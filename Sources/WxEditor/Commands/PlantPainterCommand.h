#pragma once
#include <GameEngine/Components/Renderer/Grass/GrassRendererComponent.h>
#include <Types.h>
#include <Utils/Image/Image.h>

#include <vector>

#include "Command.h"

class PlantPainterCommand : public Command
{
private:
public:
    using GrassRendererComponent = GameEngine::Components::GrassRendererComponent;
    using GrassMeshes            = std::vector<GrassRendererComponent::Ssbo>;

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
            if (entry.component.GetSsbo())
            {
                currentDatas_.push_back(Data{.component = entry.component,
                                             .snapshot  = std::move(entry.snapshot),
                                             .current   = entry.component.GetSsbo()->GetData()});
            }
        }
    }

    void redo() override
    {
        for (const auto& data : currentDatas_)
        {
            auto copyData = data.current;
            data.component.UpdateSsbo(std::move(copyData));
        }
    }

    void undo() override
    {
        for (const auto& data : currentDatas_)
        {
            auto copyData = data.snapshot;
            data.component.UpdateSsbo(std::move(copyData));
        }
    }

private:
    Datas currentDatas_;
};