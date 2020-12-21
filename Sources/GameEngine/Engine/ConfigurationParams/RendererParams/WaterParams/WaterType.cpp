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
