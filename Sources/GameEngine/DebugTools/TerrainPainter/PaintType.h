#pragma once
#include <string>

namespace GameEngine
{
enum class PaintType
{
    BlendMap,
    HeightMap
};
}  // namespace GameEngine

namespace std
{
void from_string(const std::string&, GameEngine::PaintType&);
string to_string(GameEngine::PaintType);
}  // namespace std
