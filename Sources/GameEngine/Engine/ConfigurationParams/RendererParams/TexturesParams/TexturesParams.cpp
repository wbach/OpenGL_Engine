#include "TexturesParams.h"

namespace GameEngine
{
namespace Params
{
Textures::Textures()
    : maxSize{vec2ui{4096, 4096}, {{128, 128}, {256, 256}, {512, 512}, {1024, 1024}, {2048, 2048}, {4096, 4096}, {8192, 8192}}}
    , limitTextureSize{false, {false, true}}
    , useDisplacement{true, {false, true}}
    , useAmbient{true, {false, true}}
    , useDiffuse{true, {false, true}}
    , useNormal{true, {false, true}}
    , useSpecular{true, {false, true}}
    , useRoughness{true, {false, true}}
    , useMetallic{true, {false, true}}
    , useAmientOcclusion{true, {false, true}}
    , useOpacity{true, {false, true}}
{
}
}  // namespace Params
}  // namespace GameEngine
