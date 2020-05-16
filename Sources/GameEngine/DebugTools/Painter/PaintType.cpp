#include "PaintType.h"

namespace std
{
string to_string(GameEngine::PaintType input)
{
    switch (input)
    {
        case GameEngine::PaintType::BlendMap:
            return "BlendMap";
        case GameEngine::PaintType::HeightMap:
            return "HeightMap";
        case GameEngine::PaintType::Plant:
            return "Plant";
    }
    return "unknown";
}

void from_string(const string& input, GameEngine::PaintType& output)
{
    if (input == "BlendMap")
    {
        output = GameEngine::PaintType::BlendMap;
    }
    else if (input == "HeightMap")
    {
        output = GameEngine::PaintType::HeightMap;
    }
    else if (input == "Plant")
    {
        output = GameEngine::PaintType::Plant;
    }
}
}  // namespace std
