#include "PramsConstants.h"

namespace GameEngine
{
namespace Params
{
const std::vector<vec2ui> defaultResolutions()
{
    // clang-format off
    return {
            {320, 240},
            {640, 480},
            {800, 480},
            {1000, 600},
            {1024, 768},
            {1280, 768},
            {1280, 800},
            {1280, 1024},
            {1366, 768},
            {1600, 900},
            {1650, 1050},
            {1920, 1080},
            {2560, 1080},
            {2560, 1440},
            {3840, 2160},
            {4096, 2160},
            {8192, 4096}
           };
    // clang format on
}
const vec2ui DEFAULT_WINDOW_SIZE = { 1000, 600 };
}  // namespace Params
}  // namespace GameEngine
