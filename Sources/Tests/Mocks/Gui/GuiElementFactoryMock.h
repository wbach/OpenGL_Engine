#pragma once
#include <GameEngine/Renderers/GUI/IGuiElementFactory.h>
#include <gmock/gmock.h>

namespace GameEngine
{
class GuiElementFactoryMock : public IGuiElementFactory
{
public:
    MOCK_METHOD(std::unique_ptr<GuiElement>, CreateGuiElement, (), (const, override));
    MOCK_METHOD(std::unique_ptr<GuiElement>, CreateGuiElement, (const vec2&, const vec2&), (const, override));

    MOCK_METHOD(std::unique_ptr<GuiTextElement>, CreateGuiText, (const std::string&, const std::string&, uint32, uint32),
                (override));
    MOCK_METHOD(std::unique_ptr<GuiTextElement>, CreateGuiText, (const std::string&, uint32), (override));
    MOCK_METHOD(std::unique_ptr<GuiTextElement>, CreateGuiText, (const std::string&), (override));

    MOCK_METHOD(std::unique_ptr<GuiTextureElement>, CreateGuiTexture, (const std::string&), (override));

    MOCK_METHOD(std::unique_ptr<GuiWindowElement>, CreateGuiWindow, (GuiWindowStyle, const vec2&, const vec2&), (override));
    MOCK_METHOD(std::unique_ptr<GuiWindowElement>, CreateGuiWindow, (GuiWindowStyle, const vec2&, const vec2&, const vec4&),
                (override));
    MOCK_METHOD(std::unique_ptr<GuiWindowElement>, CreateGuiWindow,
                (GuiWindowStyle, const vec2&, const vec2&, const std::string&), (override));

    MOCK_METHOD(std::unique_ptr<GuiButtonElement>, CreateGuiButton, (std::function<void(GuiElement&)>), (override));
    MOCK_METHOD(std::unique_ptr<GuiButtonElement>, CreateGuiButton, (const std::string&, std::function<void(GuiElement&)>),
                (override));

    MOCK_METHOD(std::unique_ptr<GuiEditBoxElement>, CreateEditBox, (), (override));
    MOCK_METHOD(std::unique_ptr<GuiEditBoxElement>, CreateEditBox, (const std::string&), (override));

    MOCK_METHOD(std::unique_ptr<VerticalLayout>, CreateVerticalLayout, (), (override));
    MOCK_METHOD(std::unique_ptr<HorizontalLayout>, CreateHorizontalLayout, (), (override));

    MOCK_METHOD(std::unique_ptr<TreeView>, CreateTreeView, (std::function<void(GuiElement&)>), (override));
    MOCK_METHOD(std::unique_ptr<Menu>, CreateMenu, (), (override));

    MOCK_METHOD(void, CreateMessageBox, (const std::string&, const std::string&, std::function<void()>), (override));
};

}  // namespace GameEngine
