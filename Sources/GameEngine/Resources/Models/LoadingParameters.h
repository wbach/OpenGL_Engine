#pragma once

namespace GameEngine
{
enum class ModelNormalization
{
    none,
    normalized
};
enum class MeshOptimize
{
    none,
    optimized
};
struct LoadingParameters
{
    MeshOptimize meshOptimize{MeshOptimize::optimized};
    ModelNormalization modelNormalization{ModelNormalization::normalized};
};
static const LoadingParameters DEFAULT_LOADING_PARAMETERS{MeshOptimize::optimized, ModelNormalization::normalized};
}  // namespace GameEngine
