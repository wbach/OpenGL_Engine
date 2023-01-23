#pragma once
#include <string>
#include <vector>
#include <iterator>

namespace GameEngine
{
enum class TextureBrushType
{
    CircleLinear

};
std::vector<TextureBrushType> AvaiableTextureBrushType();
std::vector<std::string> AvaiableTextureBrushTypeStrs();
}  // namespace GameEngine

namespace std
{
void from_string(const std::string&, GameEngine::TextureBrushType&);
string to_string(GameEngine::TextureBrushType);
} // namespace std

