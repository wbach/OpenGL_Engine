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
    rootWindow_ = guiFactory_.CreateGuiWindow(GuiWindowStyle::BACKGROUND_ONLY, vec2(0.85, 1.f), vec2(0.3f, 0),
                                              vec4(1.f, 1.f, 1.f, 0.5f));
    rootWindow_->SetZPosition(-11.f);
    rootWindow_->Show(EngineConf.debugParams.showRenderInfo);
    auto verticalLayout = guiFactory_.CreateVerticalLayout();
    verticalLayout->SetAlgin(GameEngine::VerticalLayout::Algin::CENTER);
    verticalLayout_ = verticalLayout.get();
    rootWindow_->AddChild(std::move(verticalLayout));

    isShowSub_ = EngineConf.debugParams.showRenderInfo.subscribeForChange([this](const auto& newValue)
    {
        rootWindow_->Show(newValue);
    });
}

GuiEngineContextManger::~GuiEngineContextManger()
{
    DEBUG_LOG("destructor");
    EngineConf.debugParams.showRenderInfo.unsubscribe(isShowSub_);
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
            text->SetAlgin(GuiTextElement::Algin::LEFT);
            text->setUniqueTextureName(measurment.first);
            guiTexts_.insert({measurment.first, text.get()});
            verticalLayout_->AddChild(std::move(text));
            AdjustSize(0.025);
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
    const float childSize = 0.025f;
    auto windowScale      = rootWindow_->GetScreenScale();
    windowScale.y         = verticalLayout_->GetChildren().size() * childSize;
    auto windowPosition   = rootWindow_->GetScreenPosition();
    windowPosition.y      = 1.f - (childSize * verticalLayout_->GetChildren().size() / 2.f);

    rootWindow_->SetScreenPostion(windowPosition);
    rootWindow_->SetScreenScale(windowScale);

    for (auto& child : verticalLayout_->GetChildren())
    {
        child->SetScreenScale({child->GetScreenScale().x, childSize});
    }
}
}  // namespace GameEngine
