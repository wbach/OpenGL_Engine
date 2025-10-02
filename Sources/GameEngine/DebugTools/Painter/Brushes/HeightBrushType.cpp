#include "HeightBrushType.h"

#include <algorithm>
#include <iterator>

namespace GameEngine
{
std::vector<HeightBrushType> AvaiableHeightBrushType()
{
    return {HeightBrushType::CircleLinear, HeightBrushType::CircleAverage, HeightBrushType::CircleConstantValue,
            HeightBrushType::CircleConstantRelative, HeightBrushType::CircleGaussianHeightBrush};
}

std::vector<std::string> AvaiableHeightBrushTypeStrs()
{
    std::vector<std::string> result;
    auto input = AvaiableHeightBrushType();

    std::transform(input.begin(), input.end(), std::back_inserter(result), [](auto hb) { return std::to_string(hb); });
    return result;
}

}  // namespace GameEngine

namespace std
{
string to_string(GameEngine::HeightBrushType input)
{
    switch (input)
    {
        case GameEngine::HeightBrushType::CircleLinear:
            return "CircleLinear";
        case GameEngine::HeightBrushType::CircleAverage:
            return "CircleAverage";
        case GameEngine::HeightBrushType::CircleConstantValue:
            return "CircleConstantValue";
        case GameEngine::HeightBrushType::CircleConstantRelative:
            return "CircleConstantRelative";
        case GameEngine::HeightBrushType::CircleGaussianHeightBrush:
            return "CircleGaussianHeightBrush";
    }
    return "unknown";
}

void from_string(const string& input, GameEngine::HeightBrushType& output)
{
    if (input == "CircleLinear")
    {
        output = GameEngine::HeightBrushType::CircleLinear;
    }
    else if (input == "CircleAverage")
    {
        output = GameEngine::HeightBrushType::CircleAverage;
    }
    else if (input == "CircleConstantValue")
    {
        output = GameEngine::HeightBrushType::CircleConstantValue;
    }
    else if (input == "CircleConstantRelative")
    {
        output = GameEngine::HeightBrushType::CircleConstantRelative;
    }
    else if (input == "CircleGaussianHeightBrush")
    {
        output = GameEngine::HeightBrushType::CircleGaussianHeightBrush;
    }
}

}  // namespace std
