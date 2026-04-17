#pragma once
#include <Params.h>
#include <Property.h>

#include "GameEngine/Renderers/GUI/HorizontalAlign.h"
#include "RenderMode.h"

namespace GameEngine
{
namespace GUI
{
class RenderTextParameters : public Params
{
public:
    Property<RenderMode> mode{RenderMode::FIT, &dirty};
    Property<HorizontalAlign> align{HorizontalAlign::CENTER, &dirty};

    RenderTextParameters() = default;
    RenderTextParameters(const RenderTextParameters& other)
        : Params(other)
        , mode(other.mode, &dirty)
        , align(other.align, &dirty)
    {
    }
};
}  // namespace GUI
}  // namespace GameEngine
