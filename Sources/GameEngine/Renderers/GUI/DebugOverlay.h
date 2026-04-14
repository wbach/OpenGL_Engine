#pragma once
#include <Types.h>
#include <Utils/Time/Stopwatch.h>

#include <memory>
#include "GameEngine/Renderers/GUI/Layer/Layer.h"

namespace Utils
{
class MeasurementHandler;
}

namespace GameEngine
{
namespace GUI
{
class IElementFactory;
class Window;
class Text;
class VerticalLayout;
class Manager;
}  // namespace GUI

class DebugOverlay
{
public:
    DebugOverlay(Utils::MeasurementHandler&, GUI::Manager&, GUI::IElementFactory&);
    ~DebugOverlay();
    void Update();

private:
    void AdjustSize(float);

private:
    Utils::MeasurementHandler& measurementHandler_;
    GUI::Manager& guiManager_;
    GUI::IElementFactory& guiFactory_;
    GUI::VerticalLayout* verticalLayout_;
    GUI::Window* rootWindow_;
    GUI::Layer* layer_;
    IdType isShowSub_;
    Utils::Stopwatch updateTimer;
};
}  // namespace GameEngine
