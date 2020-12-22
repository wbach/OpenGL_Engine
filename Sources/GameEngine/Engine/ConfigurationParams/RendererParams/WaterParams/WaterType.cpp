#include "WaterType.h"

std::string GameEngine::Params::paramToString(WaterType type)
{
    if (type == WaterType::FULL)
        return "FULL";

    if (type == WaterType::REFLECTED_REFRACTED)
        return "REFLECTED_REFRACTED";

    if (type == WaterType::SIMPLE)
        return "SIMPLE";

    return std::string();
}

bool operator<(GameEngine::Params::WaterType a, GameEngine::Params::WaterType b)
{
    return static_cast<int>(a) < static_cast<int>(b);
}
