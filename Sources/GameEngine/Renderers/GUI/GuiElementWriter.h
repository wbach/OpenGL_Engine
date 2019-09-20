#pragma once
#include <Types.h>
#include <Utils/XML/XmlWriter.h>
#include "GameEngine/Renderers/GUI/Layer/GuiLayer.h"

namespace GameEngine
{
class GuiElementWriter
{
public:
    void write(const std::string&, std::vector<GuiLayer>);
};
}  // namespace GameEngine
