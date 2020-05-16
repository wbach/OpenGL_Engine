#include "TextureBrushType.h"
#include <algorithm>

namespace GameEngine
{
std::vector<TextureBrushType> AvaiableTextureBrushType()
{
    return {TextureBrushType::CircleLinear};
}

std::vector<std::string> AvaiableTextureBrushTypeStrs()
{
    std::vector<std::string> result;
    auto input = AvaiableTextureBrushType();

    std::transform(input.begin(), input.end(), std::back_inserter(result), [](auto hb){ return std::to_string(hb);});
    return result;
}

}  // namespace GameEngine

namespace std
{
string to_string(GameEngine::TextureBrushType input)
{
    switch (input)
    {
        case GameEngine::TextureBrushType::CircleLinear:
            return "CircleLinear";
    }
    return "unknown";
}

void from_string(const string& input, GameEngine::TextureBrushType& output)
{
    if (input == "CircleLinear")
    {
        output = GameEngine::TextureBrushType::CircleLinear;
    }
}

}  // namespace std
