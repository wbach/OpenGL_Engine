#pragma once
#include <Property.h>
#include <Params.h>

#include "RenderMode.h"
#include "GameEngine/Renderers/GUI/HorizontalAlign.h"

namespace GameEngine
{
namespace GUI
{
class RenderTextParameters : public Params
{
public:
    Property<RenderMode> mode{RenderMode::FIT, &dirty};
    Property<HorizontalAlign> align{HorizontalAlign::CENTER, &dirty};
};
}  // namespace GUI
}  // namespace GameEngine
