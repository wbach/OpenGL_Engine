#pragma once
#include <string>
#include <vector>

namespace GameEngine
{
enum class StepInterpolation
{
    None,
    Linear
};

std::vector<StepInterpolation> AvaiableStepInterpolations();
std::vector<std::string> AvaiableStepInterpolationsStrs();
} // namespace GameEngine

namespace std
{
void from_string(const std::string&, GameEngine::StepInterpolation&);
string to_string(GameEngine::StepInterpolation);
} // namespace std
