#include "MaterialPresets.h"

namespace GameEngine
{
namespace MaterialPresets
{

Material Leaf()
{
    Material mat;
    mat.name               = "Leaf";
    mat.baseColor          = glm::vec4(0.95f, 1.00f, 0.95f, 1.0f);
    mat.metallicFactor     = 0.0f;
    mat.roughnessFactor    = 0.65f;
    mat.normalScale        = 0.7f;
    mat.subsurfaceStrength = 0.45f;
    mat.subsurfaceColor    = glm::vec3(0.25f, 0.55f, 0.25f);
    mat.opacityCutoff      = 0.5f;
    mat.flags              = MAT_DOUBLE_SIDED | MAT_FOLIAGE | MAT_ALPHA_TEST;
    return mat;
}

Material Trunk()
{
    Material mat;
    mat.name            = "Trunk";
    mat.baseColor       = glm::vec4(1.02f, 0.98f, 0.95f, 1.0f);
    mat.metallicFactor  = 0.0f;
    mat.roughnessFactor = 0.85f;
    mat.normalScale     = 1.0f;
    mat.flags           = 0;
    return mat;
}

Material Terrain()
{
    Material mat;
    mat.name            = "Terrain";
    mat.baseColor       = glm::vec4(0.98f, 0.97f, 0.95f, 1.0f);
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
    mat.baseColor       = glm::vec4(0.97f, 0.97f, 0.98f, 1.0f);
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
    mat.baseColor       = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    mat.metallicFactor  = 0.0f;
    mat.roughnessFactor = 0.7f;
    mat.normalScale     = 1.0f;
    mat.flags           = 0;
    return mat;
}

} // namespace MaterialPresets
} // namespace GameEngine
