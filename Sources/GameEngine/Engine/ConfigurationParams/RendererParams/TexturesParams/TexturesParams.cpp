#include "TexturesParams.h"

namespace GameEngine
{
namespace Params
{
Textures::Textures()
    : maxSize{IConfigurationParam::Type::Vec2i,
              vec2ui{4096, 4096},
              {{128, 128}, {256, 256}, {512, 512}, {1024, 1024}, {2048, 2048}, {4096, 4096}, {8192, 8192}}}
    , useDisplacement{IConfigurationParam::Type::Boolean, false, {false, true}}
    , useAmbient{IConfigurationParam::Type::Boolean, false, {false, true}}
    , useDiffuse{IConfigurationParam::Type::Boolean, true, {false, true}}
    , useNormal{IConfigurationParam::Type::Boolean, true, {false, true}}
    , useSpecular{IConfigurationParam::Type::Boolean, false, {false, true}}
{
}
}  // namespace Params
}  // namespace GameEngine
