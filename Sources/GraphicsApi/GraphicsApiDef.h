#pragma once
#include <Types.h>
#include <optional>

namespace GraphicsApi
{
const size_t MAX_BONES_PER_VERTEX          = 4;
const uint32 MAX_PARTICLE_INSTANCES        = 10000;
const uint32 PARTICLE_INSTANCE_DATA_LENGTH = 21;

typedef std::optional<uint32> ID;
}  // namespace GraphicsApi