#pragma once
#include "GameEngine/Engine/EngineContext.h"
#include <memory>

namespace GameEngine
{
class GuiElementFactory;
class GuiWindowElement;
class GuiTextElement;
class VerticalLayout;

class GuiEngineContextManger
{
public:
    GuiEngineContextManger(Utils::MeasurementHandler&, GuiElementFactory&);
    ~GuiEngineContextManger();
    void Update();

private:
    void AdjustSize(float);

private:
    Utils::MeasurementHandler& measurementHandler_;
    GuiElementFactory& guiFactory_;
    VerticalLayout* verticalLayout_;
    std::unique_ptr<GuiWindowElement> rootWindow_;
    std::unordered_map<std::string, GuiTextElement*> guiTexts_;
    IdType isShowSub_;
};
}  // namespace GameEngine
