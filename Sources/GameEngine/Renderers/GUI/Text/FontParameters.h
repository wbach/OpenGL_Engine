#pragma once
#include <EngineApi.h>
#include <Params.h>
#include <Property.h>

#include <optional>

#include "FontStyle.h"
#include "GameEngine/Resources/FileHandle.h"

namespace GameEngine
{
namespace GUI
{
class ENGINE_API FontParameters : public Params
{
public:
    Property<FontStyle> style{FontStyle::Normal, &dirty};
    Property<uint32> outline{0, &dirty};
    Property<std::optional<uint32>> size{std::nullopt, &dirty};
    Property<std::optional<FileHandle>> file{std::nullopt, &dirty};

    FontParameters() = default;
    FontParameters(const FontParameters& other)
        : Params(other)
        , style(other.style, &dirty)
        , outline(other.outline, &dirty)
        , size(other.size, &dirty)
        , file(other.file, &dirty)
    {
    }
};
}  // namespace GUI
}  // namespace GameEngine
