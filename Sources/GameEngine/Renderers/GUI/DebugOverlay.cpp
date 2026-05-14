#include "DebugOverlay.h"

#include <Logger/Log.h>
#include <Utils/MeasurementHandler.h>

#include "GameEngine/Engine/Configuration.h"
#include "IElementFactory.h"
#include "Layer/Layer.h"
#include "Layout/VerticalLayout.h"
#include "Manager.h"
#include "Text/Text.h"
#include "Types.h"
#include "Window/Window.h"

namespace GameEngine
{
namespace
{
constexpr std::string_view LAYER_NAME{"DebugOverlay"};
}
DebugOverlay::DebugOverlay(Utils::MeasurementHandler& measurementHandler, GUI::Manager& manager, GUI::IElementFactory& guiFactory)
    : measurementHandler_(measurementHandler)
    , guiManager_(manager)
    , guiFactory_(guiFactory)
{
    auto window = guiFactory_.createWindow(GUI::WindowStyle::BACKGROUND_ONLY);
    rootWindow_ = window.get();
    rootWindow_->setScreenPostion(vec2(0.85, 1.f));
    rootWindow_->setScreenScale(vec2(0.3, 0));
    rootWindow_->setBackground(Color(1.f, 1.f, 1.f, 0.5f));

    rootWindow_->setZPosition(-11.f);
    rootWindow_->activate(EngineConf.debugParams.showRenderInfo);
    auto verticalLayout = guiFactory_.createVerticalLayout();
    verticalLayout->setAlign(GUI::HorizontalAlign::CENTER);
    verticalLayout_ = verticalLayout.get();
    rootWindow_->addChild(std::move(verticalLayout));

    layer_ = &guiManager_.createLayer(LAYER_NAME);
    layer_->add(std::move(window));

    isShowSub_ = EngineConf.debugParams.showRenderInfo.subscribeForChange(
        [this]() { rootWindow_->activate(EngineConf.debugParams.showRenderInfo); });
}

DebugOverlay::~DebugOverlay()
{
    LOG_DEBUG << "destructor";
    EngineConf.debugParams.showRenderInfo.unsubscribe(isShowSub_);
}

void DebugOverlay::Update()
{
    if (not rootWindow_->isActive() or updateTimer.GetTimeMilliseconds() < 100)
    {
        return;
    }
    updateTimer.Reset();

    for (const auto& measurment : measurementHandler_.GetMeasurments())
    {
        auto printedText = measurment.first + " : " + measurment.second.value;

        auto text = verticalLayout_->get<GUI::Text>(measurment.first);
        if (not text)
        {
            auto text = guiFactory_.createText(printedText);

            text->font.size    = 40;
            text->render.align = GUI::HorizontalAlign::LEFT;
            text->setTextureColor(vec3(.1f));
            text->setLabel(measurment.first);
            verticalLayout_->addChild(std::move(text));
            AdjustSize(0.025);
        }
        else
        {
            auto& value = text->getText();

            if (value != printedText)
            {
                text->setText(printedText);
            }
        }
    }
}

void DebugOverlay::AdjustSize(float textYscale)
{
    const float childSize = 0.03f;
    auto windowScale      = rootWindow_->getScreenScale();
    windowScale.y         = verticalLayout_->getChildren().size() * childSize;
    auto windowPosition   = rootWindow_->getScreenPosition();
    windowPosition.y      = 1.f - (childSize * verticalLayout_->getChildren().size() / 2.f);

    rootWindow_->setScreenPostion(windowPosition);
    rootWindow_->setScreenScale(windowScale);

    for (auto& child : verticalLayout_->getChildren())
    {
        child->setScreenScale({child->getScreenScale().x, childSize});
    }
}
}  // namespace GameEngine
