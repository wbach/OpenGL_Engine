#pragma once
#include <Types.h>

#include <optional>
#include <variant>

#include "GameEngine/Resources/FileHandle.h"

namespace GameEngine
{
namespace GUI
{
struct Theme
{
    struct Text
    {
        Color color                     = {0.9f, 0.9f, 0.9f, 0.9f};
        uint32 fontOutline              = 0;
        std::optional<uint32> fontSize_ = std::nullopt;
        FileHandle font                 = MemoryFile{"segoe-ui.ttf"};
    };

    struct Sprite
    {
        Color color{1.f, 1.f, 1.f, 1.f};
        std::optional<FileHandle> baseTexture;
    };

    struct Button
    {
        struct StateStyle
        {
            Text text;
            Sprite sprite;
        };

        StateStyle normal{.text   = {},
                          .sprite = {.color = {1.0f, 1.0f, 1.0f, 1.0f}, .baseTexture = MemoryFile{"darkGrayButton.png"}}};
        StateStyle hover{.text   = {},
                         .sprite = {.color = {1.0f, 1.0f, 1.0f, 1.0f}, .baseTexture = MemoryFile{"darkGrayButtonHover.png"}}};
        StateStyle active{.text   = {},
                          .sprite = {.color = {1.0f, 1.0f, 1.0f, 1.0f}, .baseTexture = MemoryFile{"darkGrayButtonActive.png"}}};
        StateStyle disabled{.text   = {},
                            .sprite = {.color = {0.2f, 0.2f, 0.2f, 0.5f}, .baseTexture = MemoryFile{"darkGrayButton.png"}}};
    };

    struct EditText
    {
       Text text;
       Sprite background;
    };

    struct Window
    {
        Sprite windowBar{.color = {1.0f, 1.0f, 1.0f, 1.0f}, .baseTexture = MemoryFile{"darkGrayButton.png"}};
        Sprite background{.color = {1.0f, 1.0f, 1.0f, 1.0f}, .baseTexture = MemoryFile{"darkGrayButton.png"}};
    };

    Text text;
    EditText editText;
    Sprite sprite;
    Button button;
    Window window;
};
}  // namespace GUI
}  // namespace GameEngine
