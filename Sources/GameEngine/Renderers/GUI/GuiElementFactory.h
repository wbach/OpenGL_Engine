#pragma once
#include <functional>
#include <memory>
#include <Types.h>
#include "GuiElement.h"
#include "Text/GuiTextFactory.h"

namespace Input
{
class InputManager;
}  // namespace Input

namespace GameEngine
{
class GuiManager;
class GuiTextFactory;
class IResourceManager;
class GuiButtonElement;
class GuiTextureElement;
class GuiWindowElement;
class GuiTextElement;

class GuiElementFactory
{
public:
    struct EntryParameters
    {
        GuiManager& guiManager_;
        Input::InputManager& inputManager_;
        IResourceManager& resourceManager_;
        const vec2ui& windowSize_;
    };
    GuiElementFactory(EntryParameters& entryParameters);
    ~GuiElementFactory() = default;
    void ReadGuiFile(const std::string& filename);
    GuiTextElement* CreateGuiText(const std::string&, const std::string&, const std::string&, uint32, uint32);
    GuiTextureElement* CreateGuiTexture(const std::string&, const std::string&);
    GuiWindowElement* CreateGuiWindow(const std::string&, const Rect&, const std::string&);
    GuiButtonElement* CreateGuiButton(std::function<void()>, const std::string*, const std::string* = nullptr,
                                      const std::string* = nullptr, const std::string* = nullptr);

private:
    std::unique_ptr<GuiTextureElement> MakeGuiTexture(const std::string&);

private:
    GuiManager& guiManager_;
    Input::InputManager& inputManager_;
    IResourceManager& resourceManager_;
    const vec2ui& windowSize_;
    GuiTextFactory guiTextFactory_;
    std::string lastGuiFileMd5Value_;

private:
    uint32 guiElementCounter_;
};
}  // namespace GameEngine
