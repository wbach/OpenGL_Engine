#pragma once
#include <string>
#include <vector>

namespace GameEngine
{
enum class HeightBrushType
{
    CircleLinear,
    CircleAverage,
    CircleConstantValue
};
std::vector<HeightBrushType> AvaiableHeightBrushType();
std::vector<std::string> AvaiableHeightBrushTypeStrs();
}  // namespace GameEngine

namespace std
{
void from_string(const std::string&, GameEngine::HeightBrushType&);
string to_string(GameEngine::HeightBrushType);
} // namespace std

