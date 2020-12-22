#pragma once
#include <string>

namespace GameEngine
{
namespace Params
{
enum class WaterType
{
    SIMPLE = 0,
    REFLECTED_REFRACTED,
    FULL
};
std::string paramToString(WaterType);

}  // namespace Params
}  // namespace GameEngine

bool operator<(GameEngine::Params::WaterType, GameEngine::Params::WaterType);
