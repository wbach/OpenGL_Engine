#include "ElementFactory.h"

#include <Logger/Log.h>

#include <algorithm>
#include <memory>

#include "Button/Button.h"
#include "EditText/EditText.h"
#include "ElementsDef.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/GUI/Element.h"
#include "GameEngine/Renderers/GUI/IElementVisitor.h"
#include "GameEngine/Renderers/GUI/Sprite/Sprite.h"
#include "GameEngine/Renderers/GUI/Text/MultiLineText.h"
#include "GameEngine/Renderers/GUI/Theme.h"
#include "GameEngine/Renderers/GUI/Window/Window.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "Layout/HorizontalLayout.h"
#include "Layout/VerticalLayout.h"
#include "Manager.h"
#include "Sprite/Sprite.h"
#include "Text/Text.h"
#include "Window/Window.h"

namespace GameEngine
{
namespace GUI
{
ElementFactory::ElementFactory(ElementFactory::EntryParameters &entryParameters)
    : parameters_{entryParameters}
{
}

ElementFactory::~ElementFactory()
{
    LOG_DEBUG << "destructor";
}

std::unique_ptr<Text> ElementFactory::createText(const std::string &text)
{
    return createText(text, theme_.text);
}

std::unique_ptr<Text> ElementFactory::createText(const std::string &text, const Theme::Text &theme)
{
    auto guiText = std::make_unique<Text>(fontManger_, parameters_.resourceManager_, parameters_.renderer, text);

    guiText->setLocalScale(vec2(0.5f, 0.2f));
    guiText->font.file    = theme.font;
    guiText->font.outline = theme.fontOutline;
    guiText->font.size    = theme.fontSize_;
    guiText->setColor(theme.color);
    return guiText;
}

std::unique_ptr<MultiLineText> ElementFactory::createMultiLineText(const std::string &text)
{
    auto guiText = std::make_unique<MultiLineText>(fontManger_, parameters_.resourceManager_, parameters_.renderer, text);

    const auto &theme = theme_.text;
    // guiText->setLocalScale(vec2(0.5f, 0.2f));
    guiText->font.file    = theme.font;
    guiText->font.outline = theme.fontOutline;
    guiText->font.size    = theme.fontSize_;
    guiText->setColor(theme.color);
    return guiText;
}

std::unique_ptr<Sprite> ElementFactory::createSprite(const FileHandle &file)
{
    return createSprite(file, theme_.sprite);
}

std::unique_ptr<Sprite> ElementFactory::createSprite(const FileHandle &file, const Theme::Sprite &theme)
{
    auto sprite = makeSprite(file);
    sprite->setColor(theme.color);
    return sprite;
}

std::unique_ptr<Window> ElementFactory::createWindow(WindowStyle style)
{
    auto guiWindow = std::make_unique<Window>(style);
    if (style != WindowStyle::EMPTY)
    {
        const auto &bgTexture = theme_.window.background.baseTexture;
        if (bgTexture)
        {
            if (auto bgSprite = makeSprite(*bgTexture))
            {
                bgSprite->setLocalScale({1.f, 1.f});
                bgSprite->setColor(theme_.window.background.color);
                guiWindow->setBackground(std::move(bgSprite));
            }
        }

        if (style != WindowStyle::BACKGROUND_ONLY)
        {
            createWindowBar(style, *guiWindow);
        }
    }

    return guiWindow;
}

std::unique_ptr<Button> ElementFactory::createButton(const std::optional<std::string> &text)
{
    auto button = std::make_unique<Button>();
    button->setLocalScale(vec2(0.5f, 0.2f));
    button->setTheme(theme_.button);

    {
        const auto &normalState = theme_.button.normal;
        if (normalState.sprite.baseTexture)
        {
            if (auto sprite = createSprite(*normalState.sprite.baseTexture, normalState.sprite))
            {
                button->setBackground(std::move(sprite));
            }
        }
    }

    {
        const auto &hoverState = theme_.button.hover;
        if (hoverState.sprite.baseTexture)
        {
            if (auto sprite = createSprite(*hoverState.sprite.baseTexture, hoverState.sprite))
            {
                button->setOnHover(std::move(sprite));
            }
        }
    }

    {
        const auto &activateState = theme_.button.active;
        if (activateState.sprite.baseTexture)
        {
            if (auto sprite = createSprite(*activateState.sprite.baseTexture, activateState.sprite))
            {
                button->setOnActive(std::move(sprite));
            }
        }
    }

    if (text)
    {
        button->setText(createText(text.value()));
    }
    return button;
}

std::unique_ptr<EditText> ElementFactory::createEditText()
{
    auto text    = createText("", theme_.editText.text);
    auto cursor  = createText("|", theme_.editText.text);
    auto editBox = std::make_unique<EditText>(std::move(text), std::move(cursor), parameters_.inputManager_);

    if (auto sprite = createSprite(*theme_.editText.background.baseTexture, theme_.editText.background))
    {
        editBox->setBackground(std::move(sprite));
    }

    return editBox;
}

std::unique_ptr<VerticalLayout> ElementFactory::createVerticalLayout()
{
    return std::make_unique<VerticalLayout>();
}

std::unique_ptr<HorizontalLayout> ElementFactory::createHorizontalLayout()
{
    return std::make_unique<HorizontalLayout>();
}

std::unique_ptr<Window> ElementFactory::createMessageBox(const std::string &title, const std::string &message,
                                                         std::function<void()> okFunc)
{
    auto window = createWindow(WindowStyle::CLOSE);
    window->setScreenScale(vec2(0.5, 0.3));
    window->setZPosition(-100.f);

    auto titleText = createText(title);
    titleText->setLocalPosition(vec2(0, 0.275));
    window->addChild(std::move(titleText));

    auto messageText = createText(message);
    window->addChild(std::move(messageText));

    auto button = createButton();
    button->setText(createText("OK"));
    button->setOnClick(
        [okFunc]()
        {
            if (okFunc)
                okFunc();
        });

    button->setLocalScale(1.5f * button->getLocalScale());
    button->setLocalPosition(vec2(0, -0.25));
    window->addChild(std::move(button));
    return window;
}

void ElementFactory::setTheme(const Theme &theme)
{
    theme_ = theme;
}

const Theme &ElementFactory::getTheme() const
{
    return theme_;
}

std::unique_ptr<Sprite> ElementFactory::makeSprite(const FileHandle &file)
{
    TextureParameters params;
    params.sizeLimit = std::nullopt;;
    params.flipMode        = TextureFlip::VERTICAL;
    params.filter          = GraphicsApi::TextureFilter::LINEAR;

    auto texture = parameters_.resourceManager_.GetTextureLoader().LoadTexture(file, params);
    if (not texture)
    {
        LOG_DEBUG << "Texture not loaded : " << file;
        return std::make_unique<Sprite>(parameters_.resourceManager_, parameters_.renderer);
    }

    return std::make_unique<Sprite>(parameters_.resourceManager_, parameters_.renderer, *texture);
}

void ElementFactory::createWindowBar(WindowStyle style, Window &window)
{
    const vec2 barPosition(0, window.getLocalScale().y + GUI_WINDOW_BAR_HEIGHT);

    auto horizontalLayout = createHorizontalLayout();
    horizontalLayout->setLocalScale({0.99f, GUI_WINDOW_BAR_HEIGHT});
    horizontalLayout->setLocalPosition(barPosition);
    horizontalLayout->setAlign(HorizontalAlign::RIGHT);

    auto barButton = createButton();
    barButton->setLocalScale({1.f, 1.f});
    barButton->setLocalPosition(barPosition);

    if (theme_.window.windowBar.baseTexture)
    {
        auto barTexture = createSprite(*theme_.window.windowBar.baseTexture);
        if (barTexture)
        {
            barButton->setBackground(std::move(barTexture));
        }
    }

    horizontalLayout->addChild(std::move(barButton));

    if (style != WindowStyle::WITHOUT_BUTTONS)
    {
        auto closeButton        = createButton();
        auto closeButtonTexture = createSprite("GUI/close.png");
        if (closeButtonTexture)
        {
            closeButtonTexture->setLocalScale({1.f, 1.f});
            closeButton->setBackground(std::move(closeButtonTexture));
        }
        closeButton->setZPosition(-1.f);
        horizontalLayout->addChild(std::move(closeButton));
    }

    if (style == WindowStyle::FULL)
    {
        LOG_DEBUG << ("GuiWindowElement::Style::FULL, minimalize, maxmalize buttons not implemented.");
    }

    window.setBar(std::move(horizontalLayout));
}
}  // namespace GUI
}  // namespace GameEngine
