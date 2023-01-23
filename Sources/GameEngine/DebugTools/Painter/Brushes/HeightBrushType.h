#pragma once
#include <string>
#include <vector>
#include <iterator>

namespace GameEngine
{
enum class HeightBrushType
{
    CircleLinear,
    CircleAverage,
    CircleConstantValue,
    CircleConstantRelative,
    CircleGaussianHeightBrush
};
std::vector<HeightBrushType> AvaiableHeightBrushType();
std::vector<std::string> AvaiableHeightBrushTypeStrs();
}  // namespace GameEngine

namespace std
{
void from_string(const std::string&, GameEngine::HeightBrushType&);
string to_string(GameEngine::HeightBrushType);
} // namespace std

