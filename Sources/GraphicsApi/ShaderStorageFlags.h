#pragma once
#include <Types.h>

namespace GraphicsApi
{
enum ShaderStorageFlag : uint32
{
    WRITE      = 0x1,
    READ       = 0x2,
    INVALIDATE = 0x4,
    PERSISTENT = 0x8,
    COHERENT   = 0x10
};
}
