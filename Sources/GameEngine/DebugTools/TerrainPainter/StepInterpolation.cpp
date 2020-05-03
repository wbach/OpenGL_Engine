#include "StepInterpolation.h"
#include <algorithm>

namespace GameEngine
{
std::vector<StepInterpolation> AvaiableStepInterpolations()
{
    return {StepInterpolation::None, StepInterpolation::Linear};
}
std::vector<std::string> AvaiableStepInterpolationsStrs()
{
    std::vector<std::string> result;
    auto input = AvaiableStepInterpolations();

    std::transform(input.begin(), input.end(), std::back_inserter(result), [](auto hb) { return std::to_string(hb); });
    return result;
}
}  // namespace GameEngine

namespace std
{
string to_string(GameEngine::StepInterpolation input)
{
    switch (input)
    {
        case GameEngine::StepInterpolation::Linear:
            return "Linear";
        case GameEngine::StepInterpolation::None:
            return "None";
    }
    return "unknown";
}

void from_string(const string& input, GameEngine::StepInterpolation& output)
{
    if (input == "Linear")
    {
        output = GameEngine::StepInterpolation::Linear;
    }
    else if (input == "None")
    {
        output = GameEngine::StepInterpolation::None;
    }
}

}  // namespace std
