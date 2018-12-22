#pragma once
#include "Types.h"

namespace GameEngine
{
namespace EntityRendererDef
{
static const float DEFAULT_VIEW_DISTANCE{500.f};
static const vec3 DEFAULT_SHADOW_VARIABLES{0.f, 10.f, 10.f};
static const vec4 DEFAULT_CLIP_PLANE{0.f, 1.f, 0.f, 100000.f};
}  // namespace EntityRendererDef
}  // namespace GameEngine
