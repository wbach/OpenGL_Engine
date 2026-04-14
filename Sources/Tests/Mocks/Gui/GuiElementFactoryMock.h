#pragma once
#include <GameEngine/Renderers/GUI/Button/Button.h>
#include <GameEngine/Renderers/GUI/EditText/EditText.h>
#include <GameEngine/Renderers/GUI/IElementFactory.h>
#include <GameEngine/Renderers/GUI/Layout/HorizontalLayout.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <GameEngine/Renderers/GUI/Sprite/Sprite.h>
#include <GameEngine/Renderers/GUI/Text/MultiLineText.h>
#include <GameEngine/Renderers/GUI/Text/Text.h>
#include <GameEngine/Renderers/GUI/Window/Window.h>
#include <gmock/gmock.h>

namespace GameEngine
{
namespace GUI
{
class MockElementFactory : public IElementFactory
{
public:
    // Destruktor
    virtual ~MockElementFactory() = default;

    MOCK_METHOD(void, setTheme, (const Theme&), (override));
    MOCK_METHOD(const Theme&, getTheme, (), (const, override));

    MOCK_METHOD(std::unique_ptr<Text>, createText, (const std::string&), (override));
    MOCK_METHOD(std::unique_ptr<MultiLineText>, createMultiLineText, (const std::string&), (override));
    MOCK_METHOD(std::unique_ptr<Sprite>, createSprite, (const FileHandle&), (override));
    MOCK_METHOD(std::unique_ptr<Window>, createWindow, (WindowStyle), (override));
    MOCK_METHOD(std::unique_ptr<Button>, createButton, (const std::optional<std::string>&), (override));
    MOCK_METHOD(std::unique_ptr<EditText>, createEditText, (), (override));
    MOCK_METHOD(std::unique_ptr<VerticalLayout>, createVerticalLayout, (), (override));
    MOCK_METHOD(std::unique_ptr<HorizontalLayout>, createHorizontalLayout, (), (override));

    MOCK_METHOD(std::unique_ptr<Window>, createMessageBox, (const std::string&, const std::string&, std::function<void()>),
                (override));
};
}  // namespace GUI
}  // namespace GameEngine
