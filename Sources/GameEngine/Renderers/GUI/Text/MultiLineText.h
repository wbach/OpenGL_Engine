#pragma once
#include "FontParameters.h"
#include "GameEngine/Renderers/GUI/Element.h"
#include "GameEngine/Renderers/GUI/Layout/VerticalLayout.h"
#include "GameEngine/Renderers/GUI/RenderAble.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "IFontManager.h"
#include "Property.h"
#include "RenderTextParameters.h"
#include "Surface.h"
#include "TextParameters.h"

namespace GameEngine
{
class IResourceManager;

namespace GUI
{
class ENGINE_API MultiLineText : public Element
{
public:
    MultiLineText(IFontManager&, IResourceManager&, Renderer&, const std::string&);
    MultiLineText(const MultiLineText&);
    ~MultiLineText() override;

public:
    bool onMouseWheel(const vec2&) override;
    std::unique_ptr<Element> clone() const override;

    const std::string& getText() const;
    void setText(const std::string&);
    void append(const std::string&);
    void append(char);
    void pop();
    void setColor(const Color&);
    const Color& getColor() const;
    void refreshSelf() override;
    void update() override;
    float getScreenScaleLineHeight() const;
    void setScreenScaleLineHeight(float);

public:
    TextParameters text;
    FontParameters font;
    RenderTextParameters render;
    Property<float> lineHeight{0.2f};

private:
    void accept(IElementVisitor&) override;
    void rebuildLines();
    std::vector<std::string> splitText(const std::string&, uint32);

private:
    IFontManager& fontManager_;
    IResourceManager& resourceManager;
    GUI::Renderer& renderer;
    VerticalLayout layout;
    Color textColor;

    std::optional<std::string> currentlyText;
};
}  // namespace GUI
}  // namespace GameEngine
