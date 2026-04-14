#pragma once
#include <Params.h>
#include <Property.h>
#include <Types.h>

namespace GameEngine
{
namespace GUI
{
class TextParameters : public Params
{
public:
    Property<std::optional<uint32>> wrapWidth{std::nullopt, &dirty};
    Property<std::string> text{"", &dirty};
};
}  // namespace GUI
}  // namespace GameEngine
