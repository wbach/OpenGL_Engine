#pragma once
#include <Types.h>

namespace GameEngine
{
namespace Params
{
struct PhysicsVisualizatorParams
{
    bool useWorkredToUpdatePhysicsVisualization_{false};
    uint32 refreshRateStepDown_{0}; 
};
}  // namespace Params
}  // namespace GameEngine