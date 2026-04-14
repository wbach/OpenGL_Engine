#pragma once
#include <Types.h>

#include <functional>
#include <memory>
#include <optional>

#include "Element.h"
#include "GameEngine/Renderers/GUI/Button/Button.h"
#include "GameEngine/Renderers/GUI/Renderer.h"
#include "GameEngine/Renderers/GUI/Sprite/Sprite.h"
#include "GameEngine/Renderers/GUI/Window/Window.h"
#include "GameEngine/Resources/File.h"
#include "IElementFactory.h"
#include "Text/FontManager.h"
#include "Theme.h"
#include "Window/WindowStyle.h"

namespace Input
{
class InputManager;
}  // namespace Input

namespace GameEngine
{
class IResourceManager;

namespace Renderer
{
class RenderersManager;
}  // namespace Renderer

namespace GUI
{
class ENGINE_API ElementFactory : public IElementFactory
{
public:
    struct EntryParameters
    {
        Input::InputManager& inputManager_;
        IResourceManager& resourceManager_;
        GUI::Renderer& renderer;
    };

    ElementFactory(EntryParameters&);
    ~ElementFactory();

    void setTheme(const Theme&) override;
    const Theme& getTheme() const override;

    std::unique_ptr<Text> createText(const std::string&) override;
    std::unique_ptr<MultiLineText> createMultiLineText(const std::string&) override;
    std::unique_ptr<Sprite> createSprite(const FileHandle&) override;
    std::unique_ptr<Window> createWindow(WindowStyle) override;
    std::unique_ptr<Button> createButton(const std::optional<std::string>& = std::nullopt) override;
    std::unique_ptr<EditText> createEditText() override;
    std::unique_ptr<VerticalLayout> createVerticalLayout() override;
    std::unique_ptr<HorizontalLayout> createHorizontalLayout() override;
    std::unique_ptr<Window> createMessageBox(const std::string&, const std::string&, std::function<void()> = nullptr) override;

private:
    std::unique_ptr<Text> createText(const std::string&, const Theme::Text&);
    std::unique_ptr<Sprite> createSprite(const FileHandle&, const Theme::Sprite&);
    std::unique_ptr<Sprite> makeSprite(const FileHandle&);
    void createWindowBar(WindowStyle, Window&);

private:
    EntryParameters parameters_;
    FontManager fontManger_;
    Theme theme_;
};
}  // namespace GUI
}  // namespace GameEngine
