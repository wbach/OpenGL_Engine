#include "TexturesParams.h"

namespace GameEngine
{
namespace Params
{
Textures::Textures()
    : maxSize{IConfigurationParam::Type::Vec2i,
              vec2ui{4096, 4096},
              {{128, 128}, {256, 256}, {512, 512}, {1024, 1024}, {2048, 2048}, {4096, 4096}, {8192, 8192}}}
    , useDisplacement{false}
    , useAmbient{false}
    , useDiffuse{true}
    , useNormal{true}
    , useSpecular{false}
{
}
}  // namespace Params
}  // namespace GameEngine
