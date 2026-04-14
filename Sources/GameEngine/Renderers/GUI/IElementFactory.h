#pragma once
#include <Types.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>

#include "Button/Button.h"
#include "GameEngine/Resources/FileHandle.h"

namespace GameEngine
{

namespace GUI
{
class Element;
class Text;
class MultiLineText;
class Sprite;
class Window;
class EditText;
class VerticalLayout;
class HorizontalLayout;

struct Theme;
enum class WindowStyle;

class IElementFactory
{
public:
    virtual ~IElementFactory()            = default;
    virtual void setTheme(const Theme&)   = 0;
    virtual const Theme& getTheme() const = 0;

    virtual std::unique_ptr<Text> createText(const std::string&)                                   = 0;
    virtual std::unique_ptr<MultiLineText> createMultiLineText(const std::string&)                 = 0;
    virtual std::unique_ptr<Sprite> createSprite(const FileHandle&)                                = 0;
    virtual std::unique_ptr<Window> createWindow(WindowStyle)                                      = 0;
    virtual std::unique_ptr<Button> createButton(const std::optional<std::string>& = std::nullopt) = 0;
    virtual std::unique_ptr<EditText> createEditText()                                             = 0;
    virtual std::unique_ptr<VerticalLayout> createVerticalLayout()                                 = 0;
    virtual std::unique_ptr<HorizontalLayout> createHorizontalLayout()                             = 0;

    virtual std::unique_ptr<Window> createMessageBox(const std::string&, const std::string&, std::function<void()> = nullptr) = 0;
};
}  // namespace GUI
}  // namespace GameEngine
