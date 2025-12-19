#include "MaterialPresets.h"

namespace GameEngine
{
namespace MaterialPresets
{
Material Leaf()
{
    Material mat;
    mat.name               = "Leaf";
    mat.baseColor          = glm::vec4(0.3f, 0.8f, 0.2f, 1.0f);
    mat.metallicFactor     = 0.0f;
    mat.roughnessFactor    = 0.7f;
    mat.normalScale        = 1.0f;
    mat.subsurfaceStrength = 0.5f;
    mat.subsurfaceColor    = glm::vec3(0.1f, 0.4f, 0.1f);
    mat.opacityCutoff      = 0.5f;
    mat.flags              = MAT_DOUBLE_SIDED | MAT_FOLIAGE | MAT_ALPHA_TEST;
    return mat;
}
Material Trunk()
{
    Material mat;
    mat.name            = "Trunk";
    mat.baseColor       = glm::vec4(0.55f, 0.35f, 0.2f, 1.0f);
    mat.metallicFactor  = 0.0f;
    mat.roughnessFactor = 0.8f;
    mat.normalScale     = 1.0f;
    return mat;
}
Material Terrain()
{
    Material mat;
    mat.name            = "Terrain";
    mat.baseColor       = glm::vec4(0.45f, 0.35f, 0.25f, 1.0f);
    mat.metallicFactor  = 0.0f;
    mat.roughnessFactor = 0.9f;
    mat.normalScale     = 1.0f;
    mat.flags           = 0;
    return mat;
}
Material Rock()
{
    Material mat;
    mat.name            = "Rock";
    mat.baseColor       = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    mat.metallicFactor  = 0.0f;
    mat.roughnessFactor = 0.85f;
    mat.normalScale     = 1.0f;
    mat.flags           = 0;
    return mat;
}
Material DefaultObject()
{
    Material mat;
    mat.name            = "Default";
    mat.baseColor       = glm::vec4(1.0f);
    mat.metallicFactor  = 0.0f;
    mat.roughnessFactor = 0.7f;
    mat.normalScale     = 1.0f;
    mat.flags           = 0;
    return mat;
}
}  // namespace MaterialPresets
}  // namespace GameEngine
