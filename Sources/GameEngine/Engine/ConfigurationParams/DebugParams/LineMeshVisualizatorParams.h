#pragma once
#include <Types.h>

namespace GameEngine
{
namespace Params
{
struct LineMeshVisualizatorParams
{
    bool useWorkredToUpdateVisualization_{false};
    uint32 refreshRateStepDown_{0};
};
}  // namespace Params
}  // namespace GameEngine