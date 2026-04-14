#include "MultiLineText.h"

#include <Logger/Log.h>
#include <Utils/GLM/GLMUtils.h>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/GUI/Element.h"
#include "GameEngine/Renderers/GUI/Text/TextParameters.h"
#include "GameEngine/Resources/GpuResourceLoader.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/ITextureLoader.h"
#include "Text.h"
#include "Types.h"

namespace GameEngine
{
namespace GUI
{
MultiLineText::MultiLineText(IFontManager& fontManager, IResourceManager& resourceManager, GUI::Renderer& renderer,
                             const std::string& inputText)
    : fontManager_(fontManager)
    , resourceManager(resourceManager)
    , renderer(renderer)
{
    text.text      = inputText;
    text.wrapWidth = 30;
    layout.setParent(this);
    layout.setAlign(VerticalAlign::TOP);
}

MultiLineText::MultiLineText(const MultiLineText& other)
    : Element(other)
    , text(other.text)
    , font(other.font)
    , render(other.render)
    , fontManager_(other.fontManager_)
    , resourceManager(other.resourceManager)
    , renderer(other.renderer)
{
    layout.setParent(this);
    layout.setAlign(VerticalAlign::TOP);
}

MultiLineText::~MultiLineText()
{
}

const std::string& MultiLineText::getText() const
{
    return text.text;
}

void MultiLineText::setText(const std::string& input)
{
    if (input == text.text)
        return;

    text.text = input;
    invalidate();
}

void MultiLineText::rebuildLines()
{
    if (currentlyText == text.text and not font.isDirty() and not text.isDirty() and not lineHeight.isDirty())
    {
        return;
    }

    layout.removeAll();

    std::string fullText = text.text;

    if (fullText.empty())
        return;

    auto lines = splitText(fullText, text.wrapWidth->value_or(0));

    for (const auto& lineStr : lines)
    {
        auto lineElement = std::make_unique<GUI::Text>(fontManager_, resourceManager, renderer, lineStr);
        lineElement->setLocalScale(vec2(1.f, lineHeight.get()));
        lineElement->font   = font;
        lineElement->render = render;
        lineElement->setColor(textColor);
        layout.addChild(std::move(lineElement));
    }

    layout.refresh();

    currentlyText = fullText;
    text.clearDirty();
    font.clearDirty();
    lineHeight.clearDirty();
}

void MultiLineText::append(const std::string& input)
{
    text.text = text.text.get() + input;
    invalidate();
}

void MultiLineText::append(char c)
{
    text.text = text.text.get() + c;
    invalidate();
}

void MultiLineText::pop()
{
    if (not text.text->empty())
    {
        text.text.modify().pop_back();
        invalidate();
    }
}
void MultiLineText::accept(IElementVisitor& visitor)
{
    visitor.visit(*this);
}
std::vector<std::string> MultiLineText::splitText(const std::string& input, uint32 limit)
{
    std::vector<std::string> result;
    if (limit == 0)
    {
        result.push_back(input);
        return result;
    }

    std::stringstream ss(input);
    std::string word;
    std::string currentLine;

    while (ss >> word)
    {
        if (currentLine.length() + word.length() + 1 <= limit)
        {
            if (not currentLine.empty())
            {
                currentLine += " ";
            }
            currentLine += word;
        }
        else
        {
            if (!currentLine.empty())
                result.push_back(currentLine);

            while (word.length() > limit)
            {
                result.push_back(word.substr(0, limit));
                word = word.substr(limit);
            }
            currentLine = word;
        }
    }

    if (not currentLine.empty())
    {
        result.push_back(currentLine);
    }
    return result;
}
void MultiLineText::setColor(const Color& color)
{
    textColor = color;

    for (auto& child : layout.getChildren())
    {
        if (auto text = dynamic_cast<Text*>(child.get()))
        {
            text->setColor(color);
        }
    }
}
const Color& MultiLineText::getColor() const
{
    return textColor;
}
void MultiLineText::refreshSelf()
{
    rebuildLines();
}
void MultiLineText::update()
{
    if ((not text.text->empty() and not currentlyText) or text.isDirty() or font.isDirty() or render.isDirty())
    {
        refresh();

        render.clearDirty();
    }

    layout.update();
    layout.refresh();
}
bool MultiLineText::onMouseWheel(const vec2& v)
{
    return layout.onMouseWheel(v);
}
float MultiLineText::getScreenScaleLineHeight() const
{
    return lineHeight.get() * layout.getScreenScale().y;
}
void MultiLineText::setScreenScaleLineHeight(float v)
{
    lineHeight = v / layout.getScreenScale().y;
}
std::unique_ptr<Element> MultiLineText::clone() const
{
    return std::make_unique<MultiLineText>(*this);
}
}  // namespace GUI
}  // namespace GameEngine
