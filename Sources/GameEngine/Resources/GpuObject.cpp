#include "GpuObject.h"

namespace GameEngine
{
uint64 GpuObject::s_Id_ = 0;

GpuObject::GpuObject()
    : graphicsObjectId_(std::nullopt)
    , id_(s_Id_++)
{
}
}  // namespace GameEngine
