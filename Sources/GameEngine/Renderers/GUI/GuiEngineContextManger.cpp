#include "GuiEngineContextManger.h"
#include <Logger/Log.h>
#include "GuiElementFactory.h"
#include "Layout/VerticalLayout.h"
#include "Text/GuiTextElement.h"
#include "Window/GuiWindow.h"

namespace GameEngine
{
GuiEngineContextManger::GuiEngineContextManger(Utils::MeasurementHandler& measurementHandler,
                                               GuiElementFactory& guiFactory)
    : measurementHandler_(measurementHandler)
    , guiFactory_(guiFactory)
{
    rootWindow_ = guiFactory_.CreateGuiWindow(GuiWindowStyle::BACKGROUND_ONLY, vec2(0.75, 1.0), vec2(0.25, 0),
                                              vec4(1.f, 1.f, 1.f, 0.5f));
    rootWindow_->SetZPosition(-11.f);
    auto verticalLayout = guiFactory.CreateVerticalLayout();
    verticalLayout->SetAlgin(GameEngine::VerticalLayout::Algin::LEFT);
    verticalLayout->SetScale(rootWindow_->GetScale());
    verticalLayout_ = verticalLayout.get();
    rootWindow_->AddChild(std::move(verticalLayout));
}

GuiEngineContextManger::~GuiEngineContextManger()
{
    DEBUG_LOG("destructor");
}

void GuiEngineContextManger::Update()
{
    for (const auto& measurment : measurementHandler_.GetMeasurments())
    {
        auto printedText = measurment.first + " : " + measurment.second.value;

        if (guiTexts_.count(measurment.first) == 0)
        {
            auto text = guiFactory_.CreateGuiText(printedText, 40);
            text->SetColor(vec3(.1f));
            guiTexts_.insert({measurment.first, text.get()});
            AdjustSize(text->GetScale().y);
            verticalLayout_->AddChild(std::move(text));
        }
        else
        {
            auto text   = guiTexts_.at(measurment.first);
            auto& value = text->GetText();

            if (value != printedText)
            {
                text->SetText(printedText);
            }
        }
    }
}

void GuiEngineContextManger::AdjustSize(float textYscale)
{
    auto windowScale = rootWindow_->GetScale();
    windowScale.y += textYscale;
    auto windowPosition = rootWindow_->GetPosition();
    windowPosition.y -= textYscale;
    rootWindow_->SetPostion(windowPosition);
    rootWindow_->SetScale(windowScale);
    verticalLayout_->SetScale(rootWindow_->GetScale());
}
}  // namespace GameEngine
